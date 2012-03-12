/*
 * Md5.hpp -- MD5 message digest as described in RFC 1321
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_MD5_HPP
#define FTL_MD5_HPP

#include "types.hpp"
#include "defaults.hpp"

namespace ftl
{

class ByteArray;

class Md5
{
public:
	enum { length = 16 };
	
	Md5();
	~Md5();
	void feed(const void* buf, int bufFill);
	void finish(uint8_t sum[16]);
	Ref<ByteArray, Owner> finish();
	
private:
	void consume();
	int auxSize_, auxFill_;
	uint8_t* aux_;
	uint64_t bytesFeed_;
	uint32_t a_, b_, c_, d_;
};

} // namespace ftl

#endif // FTL_MD5_HPP
