/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "Singleton.h"
#include "InetAddressSyntax.h"
#include "UriSyntax.h"

namespace flux
{

UriSyntax::UriSyntax(SyntaxDebugFactory *debugFactory)
	: SyntaxDefinition(debugFactory)
{
	SYNTAX("uri");
	IMPORT(inetAddressSyntax(), "inet");

	DEFINE("ALPHA",
		CHOICE(
			RANGE('a', 'z'), // 0x61 - 0x7A
			RANGE('A', 'Z')  // 0x41 - 0x5A
		)
	);

	DEFINE("DIGIT",
		RANGE('0', '9') // 0x30 - 0x39
	);

	DEFINE("HEXDIG",
		CHOICE(
			INLINE("DIGIT"),
			CHOICE(
				RANGE('a', 'f'), // 0x61 - 0x66
				RANGE('A', 'F')  // 0x41 - 0x46
			)
		)
	);

	DEFINE("Delimiter",
		RANGE(":/?#[]@")
	);

	DEFINE("Punctuator",
		RANGE("!$&'()*+,;=")
	);

	/*DEFINE("Reserved",
		CHOICE(
			INLINE("Delimiter"),
			INLINE("Punctuator")
		)
	);*/

	DEFINE("Unreserved",
		CHOICE(
			INLINE("ALPHA"),
			INLINE("DIGIT"),
			RANGE("-._~")
		)
	);

	DEFINE("PathChar",
		CHOICE(
			INLINE("Unreserved"),
			INLINE("PercentEncoded"),
			INLINE("Punctuator"),
			RANGE(":@")
		)
	);

	DEFINE("PercentEncoded",
		GLUE(
			CHAR('%'),
			INLINE("HEXDIG"),
			INLINE("HEXDIG")
		)
	);

	userInfo_ =
		DEFINE("UserInfo",
			REPEAT(
				CHOICE(
					INLINE("Unreserved"),
					INLINE("PercentEncoded"),
					INLINE("Punctuator"),
					CHAR(':')
				)
			)
		);

	host_ =
		DEFINE("Host",
			CHOICE(
				REF("inet::Inet6Address"),
				REF("inet::Inet4Address"),
				REPEAT(1,
					CHOICE(
						INLINE("Unreserved"),
						INLINE("PercentEncoded"),
						INLINE("Punctuator")
					)
				)
			)
		);

	port_ =
		DEFINE("Port",
			REPEAT(1,
				INLINE("DIGIT")
			)
		);

	scheme_ =
		DEFINE("Scheme",
			GLUE(
				INLINE("ALPHA"),
				REPEAT(
					CHOICE(
						INLINE("ALPHA"),
						INLINE("DIGIT"),
						RANGE("+-.")
					)
				)
			)
		);

	authority_ =
		DEFINE("Authority",
			GLUE(
				REPEAT(0, 1,
					GLUE(
						REF("UserInfo"),
						CHAR('@')
					)
				),
				REF("Host"),
				REPEAT(0, 1,
					GLUE(
						CHAR(':'),
						REF("Port")
					)
				)
			)
		);

	path_ =
		DEFINE("Path",
			GLUE(
				REPEAT(
					INLINE("PathChar")
				),
				REPEAT(
					GLUE(
						CHAR('/'),
						REPEAT(
							INLINE("PathChar")
						)
					)
				)
			)
		);

	query_ =
		DEFINE("Query",
			REPEAT(
				CHOICE(
					INLINE("PathChar"),
					RANGE("/?")
				)
			)
		);

	fragment_ =
		DEFINE("Fragment",
			REPEAT(
				CHOICE(
					INLINE("PathChar"),
					RANGE("/?")
				)
			)
		);

	uri_ =
		DEFINE("Uri",
			GLUE(
				REPEAT(0, 1,
					GLUE(
						REF("Scheme"),
						CHAR(':')
					)
				),
				REPEAT(0, 1,
					GLUE(
						STRING("//"),
						REF("Authority")
					)
				),
				REF("Path"),
				REPEAT(0, 1,
					GLUE(
						CHAR('?'),
						REF("Query")
					)
				),
				REPEAT(0, 1,
					GLUE(
						CHAR('#'),
						REF("Fragment")
					)
				)
			)
		);

	ENTRY("Uri");
	LINK();
}

UriSyntax *uriSyntax() { return Singleton<UriSyntax>::instance(); }

} // namespace flux
