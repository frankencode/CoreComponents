 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
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
	Ref<ByteArray, Owner> array = ByteArray::create(fill);
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
