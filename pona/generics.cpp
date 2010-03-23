/*
 * generics.hpp -- generic interfaces
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "types.hpp"
#include "generics.hpp"

namespace pona
{

void Stream::read(void* buf, int bufCapa)
{
	uint8_t* buf2 = (uint8_t*)buf;
	int n = bufCapa;
	while (n > 0)
	{
		int dn = readAvail(buf2, n);
		if (dn == 0)
			PONA_THROW(StreamIoException, "Reading beyond end of input");
		buf2 += dn;
		n -= dn;
	}
}

} // namespace pona
