/*
 * InetAddressSyntax.cpp -- syntax of the Internet address literal
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include "InetAddressSyntax.hpp"

namespace ftl
{

InetAddressSyntax::InetAddressSyntax(Ref<DebugFactory> debugFactory)
{
	DEFINE_VOID("DecimalOctet",
		REPEAT(0, 3, RANGE('0', '9'))
	);
	
	DEFINE_VOID("HexWord",
		REPEAT(1, 4,
			CHOICE(
				RANGE('0', '9'),
				RANGE('a', 'f'),
				RANGE('A', 'F')
			)
		)
	);
	
	inet4Address_ =
		DEFINE("Inet4Address",
			GLUE(
				REF("DecimalOctet"),
				CHAR('.'),
				REF("DecimalOctet"),
				CHAR('.'),
				REF("DecimalOctet"),
				CHAR('.'),
				REF("DecimalOctet")
			)
		);
	
	inet6Address_ =
		DEFINE("Inet6Address",
			GLUE(
				REPEAT(1, 7,
					GLUE(
						CHOICE(
							INLINE("HexWord"),
							CHAR(':')
						),
						CHAR(':')
					)
				),
				CHOICE(
					REF("Inet4Address"),
					INLINE("HexWord")
				)
			)
		);
	
	inetAddress_ =
		DEFINE("InetAddress",
			CHOICE(
				REF("Inet4Address"),
				REF("Inet6Address")
			)
		);
	
	ENTRY("InetAddress");
#ifndef NDEBUG
	LINK();
#endif
}

} // namespace ftl
