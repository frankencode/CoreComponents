/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "InetAddressSyntax.h"

namespace flux
{

InetAddressSyntax::InetAddressSyntax(SyntaxDebugFactory *debugFactory)
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
	LINK();
}

} // namespace flux
