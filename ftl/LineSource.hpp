/*
 * LineSource.hpp -- canonical buffered data source
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_LINESOURCE_HPP
#define FTL_LINESOURCE_HPP

#include "atoms"
#include "defaults.hpp"
#include "Queue.hpp"
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
	Queue<char> cache_;
	ByteArray buf_;
};

} // namespace ftl

#endif // FTL_LINESOURCE_HPP
