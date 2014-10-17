/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Singleton>
#include <flux/net/InetAddressSyntax>

namespace flux {
namespace net {

InetAddressSyntax::InetAddressSyntax(SyntaxDebugFactory *debugFactory)
{
	DEFINE("DecimalOctet",
		REPEAT(0, 3, RANGE('0', '9'))
	);

	DEFINE("HexWord",
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
				INLINE("DecimalOctet"),
				CHAR('.'),
				INLINE("DecimalOctet"),
				CHAR('.'),
				INLINE("DecimalOctet"),
				CHAR('.'),
				INLINE("DecimalOctet")
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

const InetAddressSyntax *inetAddressSyntax() { return Singleton<InetAddressSyntax>::instance(); }

}} // namespace flux::net
