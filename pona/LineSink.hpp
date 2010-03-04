/*
 * LineSink.hpp -- canonically buffered data sink
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_LINESINK_HPP
#define PONA_LINESINK_HPP

#include "atoms"
#include "defaults.hpp"
#include "Stream.hpp"
#include "UString.hpp"

namespace pona
{

class LineSink: public Instance
{
public:
	LineSink(Ref<Stream> stream, int bufCapa = PONA_DEFAULT_BUF_CAPA, const char* eol = "\n");
	~LineSink();

	void writeLine(UString line);
	void write(UString s);

	Ref<Stream> stream() const;

private:
	Ref<Stream, Owner> stream_;
	UString eol_;
	int bufCapa_;
	uint8_t* buf_;
};

} // namespace pona

#endif // PONA_LINESINK_HPP
