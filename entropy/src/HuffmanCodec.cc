/*
 * Copyright (C) 2007-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ReplaySource>
#include <cc/CaptureSink>
#include <cc/entropy/HuffmanCodec>

namespace cc {
namespace entropy {

HuffmanCodec::HuffmanCodec():
    buffer_(CharArray::allocate(4096)),
    nodes_(Array<Node>::create(512)),
    codeMap_(Array<NodeRef>::create(256 + 512)),
    heap_(MinHeap<NodeRef>::create(codeMap_->data() + 256, 512)),
    bitStack_(Stack<uint8_t>::create(256))
{
}

void HuffmanCodec::reset()
{
    nodesFill_ = 0;
    for (NodeRef &entry: codeMap_)
        entry.node_ = 0;
}

inline void HuffmanCodec::addSymbol(int value, int count0)
{
    nodes_->at(nodesFill_) = Node {
        .parent_ = 0xFFFF,
        .leftChild_ = 0xFFFF,
        .rightChild_ = 0xFFFF,
        .value_ = int16_t(value),
        .count_ = count0
    };
    codeMap_->at(value).node_ = nodes_->data() + nodesFill_;
    ++nodesFill_;
}

void HuffmanCodec::generateCodeTable()
{
    heap_->deplete();
    for (int i = 0; i < nodesFill_; ++i)
        heap_->push(NodeRef{ .node_ = nodes_->data() + i});

    while (heap_->count() > 1)
    {
        NodeRef leftChild = heap_->pop();
        NodeRef rightChild = heap_->pop();
        leftChild.node_->parent_ = nodesFill_;
        rightChild.node_->parent_ = nodesFill_;
        nodes_->at(nodesFill_) = Node {
            .parent_ = 0,
            .leftChild_ = uint16_t(leftChild.node_ - nodes_->data()),
            .rightChild_ = uint16_t(rightChild.node_ - nodes_->data()),
            .value_ = -1,
            .count_ = leftChild.node_->count_ + rightChild.node_->count_
        };
        heap_->push(NodeRef{nodes_->data() + nodesFill_});
        ++nodesFill_;
    }

    if (heap_->count() > 0)
        root_ = heap_->pop().node_ - nodes_->data();
    else
        root_ = 0xFFFF;
}

int HuffmanCodec::encode(Stream *source, BitSink *sink)
{
    /// fill input buffer

    int bufferFill = source->read(mutate(buffer_));

    if (bufferFill == 0) {
        sink->writeUInt8(0);
        return 0;
    }

    /// count how often each symbol appears in the input buffer

    reset();
    for (int i = 0; i < bufferFill; ++i) {
        int value = buffer_->byteAt(i);
        if (!codeMap_->at(value).node_) addSymbol(value, 1);
        else ++codeMap_->at(value).node_->count_;
    }

    /// write header

    sink->writeUInt8(nodesFill_);
    sink->writeUInt16(bufferFill);

    /// write symbol table with symbol counts
    {
        heap_->deplete();
        for (int i = 0; i < nodesFill_; ++i)
            heap_->push(NodeRef{ .node_ = &nodes_->at(i) });

        int maxCountDelta = 0;
        {
            int previousCount = 0;

            for (int i = 0; i < nodesFill_; ++i) {
                NodeRef entry = heap_->pop();
                int countDelta = entry.node_->count_ - previousCount;
                if (countDelta > maxCountDelta) maxCountDelta = countDelta;
                previousCount = entry.node_->count_;
            }
        }

        int minValue = 0xFF;
        int maxValue = 0;

        for (int i = 0; i < nodesFill_; ++i) {
            Node &node = nodes_->at(i);
            if (node.value_ > maxValue) maxValue = node.value_;
            if (node.value_ < minValue) minValue = node.value_;
            heap_->push(NodeRef{ .node_ = &node });
        }

        int bitsPerSymbolValue = ilog<2>(maxValue-minValue);
        int bitsPerCountDelta = ilog<2>(maxCountDelta);

        sink->writeUInt<3>(bitsPerSymbolValue);
        sink->writeUInt<5>(bitsPerCountDelta);
        sink->writeUInt<8>(minValue);

        {
            int previousCount = 0;

            for (int i = 0; i < nodesFill_; ++i) {
                NodeRef entry = heap_->pop();
                int countDelta = entry.node_->count_ - previousCount;
                sink->writeUInt(bitsPerSymbolValue, entry.node_->value_ - minValue);
                sink->writeUInt(bitsPerCountDelta, countDelta);
                previousCount = entry.node_->count_;
            }
        }
    }

    /// generate the Huffman code table

    generateCodeTable();

    /// write Huffman encoded symbols

    for (int i = 0; i < bufferFill; ++i)
    {
        uint16_t node = codeMap_->at(buffer_->byteAt(i)).node_ - nodes_->data();

        bitStack_->deplete();
        while (node != root_) {
            uint16_t parent = nodes_->at(node).parent_;
            bitStack_->push(nodes_->at(parent).rightChild_ == node);
            node = parent;
        }

        while (bitStack_->count() > 0)
            sink->writeBit(bitStack_->pop());
    }

    return bufferFill;
}

int HuffmanCodec::decode(BitSource *source, Stream *sink)
{
    /// read header

    int symbolCount = source->readUInt8();

    if (symbolCount == 0) return 0;

    int bufferFill = source->readUInt16();

    /// read symbol table
    {
        int bitsPerSymbolValue = source->readUInt<3>();
        int bitsPerCountDelta = source->readUInt<5>();
        int minValue = source->readUInt8();

        reset();
        int count = 0;
        for (int i = 0; i < symbolCount; ++i) {
            int value = source->readUInt(bitsPerSymbolValue) + minValue;
            count += source->readUInt(bitsPerCountDelta);
            addSymbol(value, count);
        }
    }

    /// generate Huffman code table

    generateCodeTable();

    /// decode Huffman encoded symbols

    for (int i = 0; i < bufferFill; ++i) {
        uint16_t node = root_;
        while (nodes_->at(node).leftChild_ != 0xFFFF)
            node = (source->readBit()) ? nodes_->at(node).rightChild_ : nodes_->at(node).leftChild_;
        mutate(buffer_)->at(i) = nodes_->at(node).value_;
    }

    sink->write(buffer_->select(bufferFill));

    return bufferFill;
}

String HuffmanCodec::encode(const String &message)
{
    auto replaySource = ReplaySource::open(message);
    auto captureSink = CaptureSink::open();
    {
        auto bitSink = BitSink::open(captureSink);
        auto huffmanCodec = HuffmanCodec::create();
        while (huffmanCodec->encode(replaySource, bitSink));
    }
    return captureSink->collect();
}

String HuffmanCodec::decode(const String &message)
{
    auto bitSource = BitSource::open(message);
    auto captureSink = CaptureSink::open();
    auto huffmanCodec = HuffmanCodec::create();
    while (huffmanCodec->decode(bitSource, captureSink));
    return captureSink->collect();
}

}} // namespace cc::entropy
