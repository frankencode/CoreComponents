/*
 * LineSource.hpp -- canonically buffered data source
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_LINESOURCE_HPP
#define FTL_LINESOURCE_HPP

#include "atoms"
#include "defaults.hpp"
#include "CircularBuffer.hpp"
#include "String.hpp"
#include "Stream.hpp"

namespace ftl
{

class LineSource: public Source<String>
{
public:
	LineSource(Ref<Stream> stream, const char* eol = "\n", int maxLineLength = FTL_DEFAULT_BUF_CAPA);
	
	Ref<Stream> stream() const;
	
	bool read(String* line);
	String readLine();
	
private:
	bool readAvail();
	bool hasMore();
	
	Ref<Stream, Owner> stream_;
	String eol_;
	int cachedLines_;
	CircularBuffer<char> cache_;
	ByteArray buf_;
};

} // namespace ftl

#endif // FTL_LINESOURCE_HPP
