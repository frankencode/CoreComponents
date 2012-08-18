/*
 * Buffer.cpp -- low-level data buffer
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include "Exception.hpp"
#include "Buffer.hpp"

namespace ftl
{

Block::Block(int size)
	: data_(new char[size]),
	  fill_(-1)
{}

Block::~Block()
{
	delete[] data_;
}

Buffer::Buffer(int blockSize)
	: blockSize_(blockSize)
{}

void Buffer::reset(int blockSize)
{
	Ref<Block> block = tail_;
	while (block) {
		block->next_ = 0;
		block = block->prev_;
	}
	head_ = tail_ = 0;
	blockSize_ = blockSize;
}

Ref<Block> Buffer::allocate()
{
	Ref<Block, Owner> newBlock = new Block(blockSize_);
	newBlock->prev_ = tail_;
	if (!tail_) head_ = tail_ = newBlock;
	else tail_ = tail_->next_ = newBlock;
	return newBlock;
}

Ref<ByteArray, Owner> Buffer::join() const
{
	int fill = 0;
	{
		Ref<Block> block = head_;
		while (block) {
			fill += block->fill_;
			block = block->next_;
		}
	}
	Ref<ByteArray, Owner> array = ByteArray::newInstance(fill);
	{
		char *d = array->data();
		Ref<Block> block = head_;
		while (block) {
			FTL_ASSERT2((0 <= block->fill_) && (block->fill_ <= blockSize_), "Block not filled properly.");
			mem::cpy(d, block->data_, block->fill_);
			d += block->fill_;
			block = block->next_;
		}
	}
	return array;
}

} // namespace ftl
