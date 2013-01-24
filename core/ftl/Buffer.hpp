 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_BUFFER_HPP
#define FTL_BUFFER_HPP

#include "defaults.hpp"
#include "strings.hpp"
#include "ByteArray.hpp"

namespace ftl
{

class Buffer;

class Block: public Instance {
public:
	Block(int size);
	~Block();

	inline char *data() const { return data_; }
	inline int fill() const { return fill_; }
	inline void setFill(int fill) { fill_ = fill; }

private:
	friend class Buffer;

	char *data_;
	int fill_;
	Ref<Block, Owner> next_;
	Block *prev_;
};

class Buffer: public Instance
{
public:
	Buffer(int blockSize = FTL_DEFAULT_BUF_CAPA);
	void reset(int blockSize = FTL_DEFAULT_BUF_CAPA);

	inline int blockSize() const { return blockSize_; }

	Block *allocate();
	Ref<ByteArray, Owner> join() const;

private:
	int blockSize_;
	Ref<Block, Owner> head_;
	Block *tail_;
};

} // namespace ftl

#endif // FTL_BUFFER_HPP
