/*
 * Buffer.hpp -- low-level data buffer
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_BUFFER_HPP
#define FTL_BUFFER_HPP

#include "defaults.hpp"
#include "strings.hpp"
#include "Array.hpp"

namespace ftl
{

class Buffer;

class Block: public Instance {
public:
	Block(int size);
	~Block();
	
	inline char* data() const { return data_; }
	inline int fill() const { return fill_; }
	inline void setFill(int fill) { fill_ = fill; }
	
private:
	friend class Buffer;
	
	char* data_;
	int fill_;
	Ref<Block, Owner> next_;
	Ref<Block> prev_;
};

class Buffer: public Instance
{
public:
	Buffer(int blockSize = FTL_DEFAULT_BUF_CAPA);
	void reset(int blockSize = FTL_DEFAULT_BUF_CAPA);
	
	inline int blockSize() const { return blockSize_; }
	
	Ref<Block> allocate();
	Ref<ByteArray, Owner> join() const;
	
private:
	int blockSize_;
	Ref<Block, Owner> head_;
	Ref<Block> tail_;
};

} // namespace ftl

#endif // FTL_BUFFER_HPP
