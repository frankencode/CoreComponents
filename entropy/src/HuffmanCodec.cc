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

HuffmanCodec::HuffmanCodec():
    buffer_(CharArray::allocate(4096)),
    nodes_(Array<Node>::create(512)),
    codeMap_(Array<NodeRef>::create(256)),
    heap_(MinHeap<NodeRef>::create(512)),
    bitStack_(Stack<uint8_t>::create(256))
{}

void HuffmanCodec::reset()
{
    nodesFill_ = 0;
    for (NodeRef &entry: codeMap_)
        entry.node_ = 0;
}

inline void HuffmanCodec::addSymbol(int value, int count0)
{
    nodes_->at(nodesFill_) = Node {
        .parent_ = 0,
        .leftChild_ = 0,
        .rightChild_ = 0,
        .value_ = value,
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
        leftChild.node_->parent_ = nodes_->data() + nodesFill_;
        rightChild.node_->parent_ = nodes_->data() + nodesFill_;
        nodes_->at(nodesFill_) = Node {
            .parent_ = 0,
            .leftChild_ = leftChild.node_,
            .rightChild_ = rightChild.node_,
            .value_ = -1,
            .count_ = leftChild.node_->count_ + rightChild.node_->count_
        };
        heap_->push(NodeRef{nodes_->data() + nodesFill_});
        ++nodesFill_;
    }

    if (heap_->count() > 0)
        root_ = heap_->pop().node_;
    else
        root_ = nullptr;
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
        uint8_t value = buffer_->byteAt(i);
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

        int maxValue = 0;

        for (int i = 0; i < nodesFill_; ++i) {
            Node &node = nodes_->at(i);
            if (node.value_ > maxValue) maxValue = node.value_;
            heap_->push(NodeRef{ .node_ = &node });
        }

        int bitsPerSymbolValue = ilog<2>(maxValue);
        int bitsPerCountDelta = ilog<2>(maxCountDelta);

        sink->writeUInt<3>(bitsPerSymbolValue);
        sink->writeUInt<5>(bitsPerCountDelta);

        {
            int previousCount = 0;

            for (int i = 0; i < nodesFill_; ++i) {
                NodeRef entry = heap_->pop();
                int countDelta = entry.node_->count_ - previousCount;
                sink->writeUInt(bitsPerSymbolValue, entry.node_->value_);
                sink->writeUInt(bitsPerSymbolValue, countDelta);
                previousCount = entry.node_->count_;
            }
        }
    }

    /// generate the Huffman code table

    generateCodeTable();

    /// write Huffman encoded symbols

    for (int i = 0; i < bufferFill; ++i)
    {
        Node *node = codeMap_->at(buffer_->byteAt(i)).node_;

        bitStack_->deplete();
        while (node->parent_) {
            Node *parent = node->parent_;
            bitStack_->push(parent->rightChild_ == node);
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

        reset();
        int count = 0;
        for (int i = 0; i < symbolCount; ++i) {
            int value = source->readUInt(bitsPerSymbolValue);
            count += source->readUInt(bitsPerCountDelta);
            addSymbol(value, count);
        }
    }

    /// generate Huffman code table

    generateCodeTable();

    /// decode Huffman encoded symbols

    for (int i = 0; i < bufferFill; ++i) {
        Node *node = root_;
        while (node->leftChild_)
            node = (source->readBit()) ? node->rightChild_ : node->leftChild_;
        mutate(buffer_)->at(i) = node->value_;
    }

    sink->write(buffer_->select(bufferFill));

    return bufferFill;
}

}} // namespace cc::entropy
