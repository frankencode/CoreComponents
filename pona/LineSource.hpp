/*
 * LineSource.hpp -- canonically buffered data source
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_LINESOURCE_HPP
#define PONA_LINESOURCE_HPP

#include "atoms"
#include "defaults.hpp"
#include "Stream.hpp"
#include "Queue.hpp"
#include "String.hpp"

namespace pona
{

class LineSource: public Instance
{
public:
	LineSource(Ref<Stream> stream, int bufCapa = PONA_DEFAULT_BUF_CAPA, String eol = "\n");
	~LineSource();
	
	String readLine(bool* eoi = 0);
	
	int cachedLines() const;
	void readAvail(bool* eoi);
	
	Ref<Stream> stream() const;
	
private:
	Ref<Stream, Owner> stream_;
	String eol_;
	int cachedLines_;
	Queue<char> cache_;
	int bufCapa_;
	char* buf_;
};

} // namespace pona

#endif // PONA_LINESOURCE_HPP
