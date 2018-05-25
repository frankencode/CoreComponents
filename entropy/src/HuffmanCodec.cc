/*
 * Copyright (C) 2007-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

// #define CC_ENTROPY_HUFFMANCODEC_PROFILING
// #define CC_ENTROPY_HUFFMANCODEC_DEBUG_STATISTICS

#ifdef CC_HUFFMANCODEC_PROFILING
#include <cc/debug>
#else
#ifdef FTL_HUFFMANCODEC_DEBUG_STATISTICS
#include <cc/debug>
#endif
#endif

#include <cc/entropy/HuffmanCodec>

namespace cc {
namespace entropy {

HuffmanCodec::HuffmanCodec(int rawDiversity, int rawDynamicRange):
    rawDiversity_(rawDiversity),
    rawDynamicRange_(rawDynamicRange),
    codeTable_(new SymbolNode[2 * rawDiversity]),
    codeMap_(new SymbolRef[rawDynamicRange_]),
    heap_(MaxHeap<SymbolRef>::create(2 * rawDiversity)),
    bitStack_(Stack<uint8_t>::create(rawDiversity))
{
    memoryConsumption();
}

HuffmanCodec::~HuffmanCodec()
{
    delete[] codeMap_; codeMap_ = 0;
    delete[] codeTable_; codeTable_ = 0;
}

int HuffmanCodec::memoryConsumption() const
{
    int ct = sizeof(SymbolNode) * 2 * rawDiversity_;
    int cm = sizeof(SymbolRef) * rawDynamicRange_;
    int sh = sizeof(SymbolRef) * heap_->capacity();
    int bs = rawDiversity_;

#ifdef CC_ENTROPY_HUFFMANCODEC_PROFILING
    CC_DEBUG <<
        (
            Format(
                ""
                "  size of code table = %%"
                "  size of code map = %%"
                "  size of sort heap = %%"
                "  size of bit stack = %%"
                "  overall heap utilization = %%"
            )
                << ct << cm << sh << bs
                << (ct + cm + sh + bs)
        )->join("\n");
#endif

    return ct + cm + sh + bs;
}

void HuffmanCodec::reset()
{
    codeTableFill_ = 0;
    for (int i = 0; i < rawDynamicRange_; ++i)
        codeMap_[i].symbol = 0;
}

inline void HuffmanCodec::addSymbol(int x, int count0)
{
    codeTable_[codeTableFill_].parent = 0;
    codeTable_[codeTableFill_].leftChild = 0;
    codeTable_[codeTableFill_].rightChild = 0;
    codeTable_[codeTableFill_].count = count0;
    codeTable_[codeTableFill_].value = x;
    codeMap_[x].symbol = codeTable_ + codeTableFill_;
    ++codeTableFill_;
}

void HuffmanCodec::generateCodeTable()
{
    heap_->deplete();
    for (int i = 0; i < codeTableFill_; ++i)
        heap_->push(SymbolRef(codeTable_ + i));

    while (heap_->count() > 1)
    {
        SymbolRef leftChild = heap_->pop();
        SymbolRef rightChild = heap_->pop();
        leftChild.symbol->parent = codeTable_ + codeTableFill_;
        rightChild.symbol->parent = codeTable_ + codeTableFill_;
        codeTable_[codeTableFill_].parent = 0;
        codeTable_[codeTableFill_].leftChild = leftChild.symbol;
        codeTable_[codeTableFill_].rightChild = rightChild.symbol;
        codeTable_[codeTableFill_].count = leftChild.symbol->count + rightChild.symbol->count;
        codeTable_[codeTableFill_].value = -1;
        heap_->push(SymbolRef(codeTable_ + codeTableFill_));
        ++codeTableFill_;
    }

    if (heap_->count() > 0)
        codeTableRoot_ = heap_->pop().symbol;
    else
        codeTableRoot_ = 0;
}

void HuffmanCodec::writeRawFrame(BitSink *sink, int *raw, int rawFill, int rawMin, int rawMax)
{
    /** write header
      */
    sink->writeUIntVlc(rawFill);
    sink->writeIntVlc(rawMin);
    sink->writeIntVlc(rawMax);
    sink->writeBit(0);    // encoding flag

    const int bits = ilog2(rawMax - rawMin + 1);
    for (int i = 0; i < rawFill; ++i)
        sink->writeUInt(bits, raw[i] - rawMin);
}

void HuffmanCodec::encode(BitSink *sink, int *raw, int rawFill, bool *userFallback)
{
    /** determine dynamic range (min, max)
      */
    int rawMin = raw[0];
    int rawMax = raw[0];
    {
        for (int i = 1; i < rawFill; ++i)
        {
            int x = raw[i];
            if (x < rawMin) rawMin = x;
            else if (x > rawMax) rawMax = x;
        }
    }

    /** optional fallback to raw data transmission
      */
    if (rawMax - rawMin + 1 > rawDynamicRange_)
    {
#ifdef CC_ENTROPY_HUFFMANCODEC_PROFILING
        CC_DEBUG <<
            Format(
                "Fallback to unencoded transmission, because out of dynamic range.\n"
                "  (rawMax - rawMin + 1, rawDynamicRange_) = (%%, %%)"
            ) << (rawMax - rawMin + 1) << rawDynamicRange_;
#endif
        if (!userFallback)
            writeRawFrame(sink, raw, rawFill, rawMin, rawMax);
        else
            *userFallback = true;
        return;
    }

    /** determine symbol frequencies
      */
    reset();
    for (int i = 0; (i < rawFill) && (codeTableFill_ < rawDiversity_); ++i)
    {
        int x = raw[i] - rawMin;
        if (!codeMap_[x].symbol) addSymbol(x, 0);
        ++codeMap_[x].symbol->count;
    }

    /** optional fallback to raw data transmission
      */
    if (codeTableFill_ == rawDiversity_)
    {
        if (!userFallback)
            writeRawFrame(sink, raw, rawFill, rawMin, rawMax);
        else
            *userFallback = true;
        return;
    }

    /** generate code table
      */
    int diversity = codeTableFill_;
    generateCodeTable();

#ifdef CC_ENTROPY_HUFFMANCODEC_DEBUG_STATISTICS
    for (int i = 0; i < diversity; ++i)
        CC_DEBUG << i << ": H(" << codeTable_[i].value << ") = " << codeTable_[i].count;
#endif

    /** determine output size
      */
    int tableSize = 0;
    int outSize = 1;    // encoding flag
    outSize += BitSink::bitsPerUIntVlc(rawFill);
    outSize += BitSink::bitsPerUIntVlc(rawMin);
    tableSize += BitSink::bitsPerUIntVlc(diversity);
    for (int i = 0; i < diversity; ++i)
    {
        tableSize += BitSink::bitsPerUIntVlc(codeTable_[i].value);
        tableSize += BitSink::bitsPerUIntVlc(codeTable_[i].count);
        SymbolNode *sym = codeTable_ + i;
        int len = 0;
        while ((sym = sym->parent) != 0) ++len;
        outSize += len * codeTable_[i].count;
    }

    outSize += tableSize;
    int outSizeBytes = outSize / 8 + (outSize % 8 != 0);

#ifdef CC_ENTROPY_HUFFMANCODEC_PROFILING
    CC_DEBUG
        << "diversity, outSize, table weight = "
        << diversity << ", " <<  outSizeBytes << ", " << double(tableSize) / outSize;
#endif // FTL_HUFFMANCODEC_PROFILING

    /** optional fallback to raw data transmission
      */
    if (outSizeBytes > encodedCapacity(rawFill, rawMax - rawMin + 1))
    {
#ifdef CC_ENTROPY_HUFFMANCODEC_PROFILING
        CC_DEBUG << "fallback to unencoded transmission";
#endif
        if (!userFallback)
            writeRawFrame(sink, raw, rawFill, rawMin, rawMax);
        else
            *userFallback = true;
        return;
    }

    if (userFallback) *userFallback = false;

    /** write header
      */
    sink->writeUIntVlc(rawFill);
    sink->writeIntVlc(rawMin);
    sink->writeIntVlc(rawMax);
    sink->writeBit(1);    // encoding flag

    /** write frequency table
      */
    sink->writeUIntVlc(diversity);
    for (int i = 0; i < diversity; ++i)
    {
        sink->writeUIntVlc(codeTable_[i].value);
        sink->writeUIntVlc(codeTable_[i].count);
    }

    /** encode symbols
      */
    for (int i = 0; i < rawFill; ++i)
    {
        int x = raw[i] - rawMin;

        SymbolNode *sym = codeMap_[x].symbol;
        bitStack_->deplete();

        while (sym->parent) {
            SymbolNode *parent = sym->parent;
            bitStack_->push(parent->rightChild == sym);
            sym = parent;
        }

        while (bitStack_->count() > 0)
            sink->writeBit(bitStack_->pop());
    }
}

int HuffmanCodec::decode(int *raw, int rawCapacity, BitSource *source)
{
    /** read header
      */
    int rawFill = source ->readUIntVlc();
    int rawMin = source->readIntVlc();
    int rawMax = source->readIntVlc();
    int encodingFlag = source->readBit();

    if (rawFill > rawCapacity) throw BufferOverflow();

    /** optional fallback to raw data transmission
      */
    if (encodingFlag == 0)
    {
        const int bits = ilog2(rawMax - rawMin + 1);
        for (int i = 0; i < rawFill; ++i)
            raw[i] = int(source->readUInt(bits)) + rawMin;
        return rawFill;
    }

    /** read frequency table
      */
    reset();
    int diversity = source ->readUIntVlc();
    for (int i = 0; i < diversity; ++i)
    {
        int value = source->readUIntVlc();
        int count = source->readUIntVlc();
        addSymbol(value, count);
    }

    /** generate code table
      */
    generateCodeTable();

    /** decode symbols
      */
    for (int i = 0; i < rawFill; ++i)
    {
        SymbolNode *sym = codeTableRoot_;
        while (sym->leftChild)
            sym = (source->readBit()) ?  sym->rightChild : sym->leftChild;
        raw[i] = sym->value + rawMin;
    }

    return rawFill;
}

int HuffmanCodec::encodedCapacity(int rawCapacity, int rawDynamicRange) const
{
    int bytesPerSymbol = 1;
    bytesPerSymbol += ((rawDynamicRange >> 8) > 1);
    bytesPerSymbol += ((rawDynamicRange >> 16) > 1);
    bytesPerSymbol += ((rawDynamicRange >> 24) > 1);

    int h = 36 * rawCapacity * bytesPerSymbol;
    h = h / 32 + (h % 32 != 0);
    h += 32;    // max frame header size
    return h;
}

#if 0
int HuffmanCodec::encode(uint8_t *encoded, int encodedCapacity, int *raw, int rawFill, bool *userFallback)
{
    Ref<BitSink> sink = BitSink::open(encoded, encodedCapacity);
    encode(sink, raw, rawFill, userFallback);
    return int(sink->numBytesWritten());
}

int HuffmanCodec::decode(int *raw, int rawCapacity, uint8_t *encoded, int encodedFill)
{
    Ref<BitSource> source = BitSource::open(encoded, encodedFill);
    return decode(raw, rawCapacity, source);
}
#endif

}} // namespace cc::entropy
