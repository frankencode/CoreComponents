/*
 * Md5.hpp -- MD5 message digest as described in RFC 1321
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_MD5_HPP
#define FTL_MD5_HPP

#include "types.hpp"
#include "defaults.hpp"

namespace ftl
{

class Md5
{
public:
	Md5();
	~Md5();
	void feed(const void* buf, int bufSize);
	void finish(uint8_t sum[16]);
	
private:
	void consume();
	int auxSize_, auxFill_;
	uint8_t* aux_;
	uint64_t bytesFeed_;
	uint32_t a_, b_, c_, d_;
};

} // namespace ftl

#endif // FTL_MD5_HPP
