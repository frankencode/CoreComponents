/*
 * LineSink.hpp -- canonically buffered data sink
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_LINESINK_HPP
#define PONA_LINESINK_HPP

#include "atom"
#include "defaults.hpp"
#include "Stream.hpp"
#include "String.hpp"

namespace pona
{

class LineSink: public Instance
{
public:
	LineSink(Ref<Stream> stream, int bufCapa = PONA_DEFAULT_BUF_CAPA, String eol = "\n");
	~LineSink();

	void writeLine(String line);
	void write(String buf);

	Ref<Stream> stream() const;

private:
	Ref<Stream, Owner> stream_;
	String eol_;
	int bufCapa_;
	uint8_t* buf_;
};

} // namespace pona

#endif // PONA_LINESINK_HPP
