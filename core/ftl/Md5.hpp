 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
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
	void feed(const void *buf, int bufFill);
	void finish(uint8_t sum[16]);
	hook<ByteArray> finish();

private:
	void consume();
	int auxSize_, auxFill_;
	uint8_t *aux_;
	uint64_t bytesFeed_;
	uint32_t a_, b_, c_, d_;
};

} // namespace ftl

#endif // FTL_MD5_HPP
