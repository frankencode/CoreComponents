/*
 * UriSyntax.cpp -- URI syntax according to RFC3986
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "UriSyntax.hpp"

namespace ftl
{

UriSyntax::UriSyntax(Ref<DebugFactory> debugFactory)
	: AbnfDefinition(debugFactory)
{
	SYNTAX("uri");
	
	DEFINE_VOID("ALPHA",
		CHOICE(
			RANGE('a', 'z'),
			RANGE('A', 'Z')
		)
	);
	
	DEFINE_VOID("DIGIT",
		RANGE('0', '9')
	);
	
	DEFINE_VOID("HEXDIG",
		CHOICE(
			INLINE("DIGIT"),
			CHOICE(
				RANGE('a', 'f'),
				RANGE('A', 'F')
			)
		)
	);
	
	DEFINE("uri",
		GLUE(
			REF("scheme"),
			CHAR(':'),
			REF("hier-part"),
			REPEAT(0, 1,
				GLUE(
					CHAR('?'),
					REF("query")
				)
			),
			REPEAT(0, 1,
				GLUE(
					CHAR('#'),
					REF("fragment")
				)
			)
		)
	);
	
	DEFINE("hier-part",
		CHOICE(
			GLUE(
				STRING("//"),
				REF("authority"),
				REF("path-abempty")
			),
			REF("path-absolute"),
			REF("path-rootless"),
			REF("path-empty")
		)
	);
	
	DEFINE("scheme",
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
	
	DEFINE("authority",
		GLUE(
			REPEAT(0, 1,
				GLUE(
					REF("userinfo"),
					CHAR('@')
				)
			),
			REF("host"),
			REPEAT(0, 1,
				GLUE(
					CHAR(':'),
					REF("port")
				)
			)
		)
	);
	
	DEFINE("userinfo",
		REPEAT(
			CHOICE(
				INLINE("unreserved"),
				REF("pct-encoded"),
				INLINE("sub-delims"),
				CHAR(':')
			)
		)
	);
	
	DEFINE("host",
		CHOICE(
			REF("inet6-address"),
			REF("inet-address"),
			REF("reg-name")
		)
	);
	
	DEFINE("port",
		REPEAT(
			INLINE("DIGIT")
		)
	);
	
	DEFINE("inet6-address",
		CHOICE(
			GLUE(
				REPEAT(6, 6,
					GLUE(
						REF("h16"),
						CHAR(':')
					)
				),
				REF("ls32")
			),
			GLUE(
				STRING("::"),
				REPEAT(5, 5,
					GLUE(
						REF("h16"),
						CHAR(':')
					)
				),
				REF("ls32")
			),
			GLUE(
				REPEAT(0, 1,
					REF("h16")
				),
				STRING("::"),
				REPEAT(4, 4,
					GLUE(
						REF("h16"),
						CHAR(':')
					)
				),
				REF("ls32")
			),
			GLUE(
				REPEAT(0, 1,
					GLUE(
						REPEAT(0, 1,
							GLUE(
								REF("h16"),
								CHAR(':')
							)
						),
						REF("h16")
					)
				),
				STRING("::"),
				REPEAT(3, 3,
					GLUE(
						REF("h16"),
						CHAR(':')
					)
				),
				REF("ls32")
			),
			GLUE(
				REPEAT(0, 1,
					GLUE(
						REPEAT(0, 2,
							GLUE(
								REF("h16"),
								CHAR(':')
							)
						),
						REF("h16")
					)
				),
				STRING("::"),
				REPEAT(2, 2,
					GLUE(
						REF("h16"),
						CHAR(':')
					)
				),
				REF("ls32")
			),
			GLUE(
				REPEAT(0, 1,
					GLUE(
						REPEAT(0, 3,
							GLUE(
								REF("h16"),
								CHAR(':')
							)
						),
						REF("h16")
					)
				),
				STRING("::"),
				REF("h16"),
				CHAR(':'),
				REF("ls32")
			),
			GLUE(
				REPEAT(0, 1,
					GLUE(
						REPEAT(0, 4,
							GLUE(
								REF("h16"),
								CHAR(':')
							)
						),
						REF("h16")
					)
				),
				STRING("::"),
				REF("ls32")
			),
			GLUE(
				REPEAT(0, 1,
					GLUE(
						REPEAT(0, 5,
							GLUE(
								REF("h16"),
								CHAR(':')
							)
						),
						REF("h16")
					)
				),
				STRING("::"),
				REF("h16")
			),
			GLUE(
				REPEAT(0, 1,
					GLUE(
						REPEAT(0, 6,
							GLUE(
								REF("h16"),
								CHAR(':')
							)
						),
						REF("h16")
					)
				),
				STRING("::")
			)
		)
	);
	
	DEFINE("h16",
		REPEAT(1, 4,
			INLINE("HEXDIG")
		)
	);
	
	DEFINE("ls32",
		CHOICE(
			GLUE(
				REF("h16"),
				CHAR(':'),
				REF("h16")
			),
			REF("inet-address")
		)
	);
	
	DEFINE("inet-address",
		GLUE(
			REF("dec-octet"),
			CHAR('.'),
			REF("dec-octet"),
			CHAR('.'),
			REF("dec-octet"),
			CHAR('.'),
			REF("dec-octet")
		)
	);
	
	DEFINE("dec-octet",
		CHOICE(
			INLINE("DIGIT"),
			GLUE(
				RANGE('1', '9'),
				INLINE("DIGIT")
			),
			GLUE(
				CHAR('1'),
				REPEAT(2, 2,
					INLINE("DIGIT")
				)
			),
			GLUE(
				CHAR('2'),
				RANGE('0', '4'),
				INLINE("DIGIT")
			),
			GLUE(
				STRING("25"),
				RANGE('0', '5')
			)
		)
	);
	
	DEFINE("reg-name",
		REPEAT(
			CHOICE(
				INLINE("unreserved"),
				REF("pct-encoded"),
				INLINE("sub-delims")
			)
		)
	);
	
	DEFINE("path-abempty",
		REPEAT(
			GLUE(
				CHAR('/'),
				REF("segment")
			)
		)
	);
	
	DEFINE("path-absolute",
		GLUE(
			CHAR('/'),
			REPEAT(0, 1,
				GLUE(
					REF("segment-nz"),
					REPEAT(
						GLUE(
							CHAR('/'),
							REF("segment")
						)
					)
				)
			)
		)
	);
	
	DEFINE("path-rootless",
		GLUE(
			REF("segment-nz"),
			REPEAT(
				GLUE(
					CHAR('/'),
					REF("segment")
				)
			)
		)
	);
	
	DEFINE("path-empty",
		PASS()
	);
	
	DEFINE("segment",
		REPEAT(
			REF("pchar")
		)
	);
	
	DEFINE("segment-nz",
		REPEAT(1,
			REF("pchar")
		)
	);
	
	DEFINE_VOID("pchar",
		CHOICE(
			INLINE("unreserved"),
			REF("pct-encoded"),
			INLINE("sub-delims"),
			RANGE(":@")
		)
	);
	
	DEFINE("query",
		REPEAT(
			CHOICE(
				REF("pchar"),
				RANGE("/?")
			)
		)
	);
	
	DEFINE("fragment",
		REPEAT(
			CHOICE(
				REF("pchar"),
				RANGE("/?")
			)
		)
	);
	
	DEFINE("pct-encoded",
		GLUE(
			CHAR('%'),
			INLINE("HEXDIG"),
			INLINE("HEXDIG")
		)
	);
	
	DEFINE_VOID("unreserved",
		CHOICE(
			INLINE("ALPHA"),
			INLINE("DIGIT"),
			RANGE("-._~")
		)
	);
	
	DEFINE_VOID("sub-delims",
		RANGE("!$&'()*+,;=")
	);
	
	ENTRY("uri");
#ifndef NDEBUG
	LINK();
#endif
}

} // namespace ftl
