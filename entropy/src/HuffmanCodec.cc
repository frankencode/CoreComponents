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
    nodes_(Array<Node>::create(2 * rawDiversity)),
    codeMap_(Array<NodeRef>::create(rawDynamicRange_)),
    heap_(MaxHeap<NodeRef>::create(2 * rawDiversity)),
    bitStack_(Stack<uint8_t>::create(rawDiversity))
{}

void HuffmanCodec::reset()
{
    fill_ = 0;
    for (int i = 0; i < rawDynamicRange_; ++i)
        codeMap_->at(i).node_ = 0;
}

inline void HuffmanCodec::addSymbol(int value, int count0)
{
    nodes_->at(fill_) = Node {
        .parent_ = 0,
        .leftChild_ = 0,
        .rightChild_ = 0,
        .value_ = value,
        .count_ = count0
    };
    codeMap_->at(value).node_ = nodes_->data() + fill_;
    ++fill_;
}

void HuffmanCodec::generateCodeTable()
{
    heap_->deplete();
    for (int i = 0; i < fill_; ++i)
        heap_->push(NodeRef{ .node_ = nodes_->data() + i});

    while (heap_->count() > 1)
    {
        NodeRef leftChild = heap_->pop();
        NodeRef rightChild = heap_->pop();
        leftChild.node_->parent_ = nodes_->data() + fill_;
        rightChild.node_->parent_ = nodes_->data() + fill_;
        nodes_->at(fill_) = Node {
            .parent_ = 0,
            .leftChild_ = leftChild.node_,
            .rightChild_ = rightChild.node_,
            .value_ = -1,
            .count_ = leftChild.node_->count_ + rightChild.node_->count_
        };
        heap_->push(NodeRef{nodes_->data() + fill_});
        ++fill_;
    }

    if (heap_->count() > 0)
        root_ = heap_->pop().node_;
    else
        root_ = nullptr;
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
    for (int i = 0; (i < rawFill) && (fill_ < rawDiversity_); ++i)
    {
        int x = raw[i] - rawMin;
        if (!codeMap_->at(x).node_) addSymbol(x, 0);
        ++codeMap_->at(x).node_->count_;
    }

    /** optional fallback to raw data transmission
      */
    if (fill_ == rawDiversity_)
    {
        if (!userFallback)
            writeRawFrame(sink, raw, rawFill, rawMin, rawMax);
        else
            *userFallback = true;
        return;
    }

    /** generate code table
      */
    int diversity = fill_;
    generateCodeTable();

#ifdef CC_ENTROPY_HUFFMANCODEC_DEBUG_STATISTICS
    for (int i = 0; i < diversity; ++i)
        CC_DEBUG << i << ": H(" << nodes_->at(i).value << ") = " << nodes_->at(i).count;
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
        tableSize += BitSink::bitsPerUIntVlc(nodes_->at(i).value_);
        tableSize += BitSink::bitsPerUIntVlc(nodes_->at(i).count_);
        int len = 0;
        Node *node = nodes_->data() + i;
        while ((node = node->parent_) != 0) ++len;
        outSize += len * nodes_->at(i).count_;
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
        sink->writeUIntVlc(nodes_->at(i).value_);
        sink->writeUIntVlc(nodes_->at(i).count_);
    }

    /** encode symbols
      */
    for (int i = 0; i < rawFill; ++i)
    {
        int x = raw[i] - rawMin;

        Node *node = codeMap_->at(x).node_;
        bitStack_->deplete();

        while (node->parent_) {
            Node *parent = node->parent_;
            bitStack_->push(parent->rightChild_ == node);
            node = parent;
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
        Node *node = root_;
        while (node->leftChild_)
            node = (source->readBit()) ?  node->rightChild_ : node->leftChild_;
        raw[i] = node->value_ + rawMin;
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

}} // namespace cc::entropy
