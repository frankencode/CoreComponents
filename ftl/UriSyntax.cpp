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
	: AbnfCoreSyntax(debugFactory)
{
	SYNTAX("uri");
	
	DEFINE_VOID("gen-delims",
		RANGE(":/?#[]@")
	);
	
	DEFINE_VOID("sub-delims",
		RANGE("!$&'()*+,;=")
	);
	
	DEFINE_VOID("reserved",
		CHOICE(
			INLINE("gen-delims"),
			INLINE("sub-delims")
		)
	);
	
	hierPart_ =
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
	
	uriReference_ =
		DEFINE("uri-reference",
			CHOICE(
				REF("uri"),
				REF("relative-ref")
			)
		);
	
	absoluteUri_ =
		DEFINE("absolute-uri",
			GLUE(
				REF("scheme"),
				CHAR(':'),
				REF("hier-part"),
				REPEAT(0, 1,
					GLUE(
						CHAR('?'),
						REF("query")
					)
				)
			)
		);
	
	relativeRef_ =
		DEFINE("relative-ref",
			GLUE(
				REF("relative-part"),
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
	
	relativePart_ =
		DEFINE("relative-part",
			CHOICE(
				GLUE(
					STRING("//"),
					REF("authority"),
					REF("path-abempty")
				),
				REF("path-absolute"),
				REF("path-noscheme"),
				REF("path-empty")
			)
		);
	
	scheme_ =
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
	
	userInfo_ =
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
	
	host_ =
		DEFINE("host",
			CHOICE(
				REF("inet6-address"),
				REF("inet-address"),
				REF("reg-name")
			)
		);
	
	port_ =
		DEFINE("port",
			REPEAT(
				INLINE("DIGIT")
			)
		);
	
	authority_ =
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
	
	decOctet_ =
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
	
	inetAddress_ =
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
	
	h16_ =
		DEFINE("h16",
			REPEAT(1, 4,
				INLINE("HEXDIG")
			)
		);
	
	ls32_ =
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
	
	inet6Address_ =
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
	
	regName_ =
		DEFINE("reg-name",
			REPEAT(
				CHOICE(
					INLINE("unreserved"),
					REF("pct-encoded"),
					INLINE("sub-delims")
				)
			)
		);
	
	path_ =
		DEFINE("path",
			CHOICE(
				REF("path-abempty"),
				REF("path-absolute"),
				REF("path-noscheme"),
				REF("path-rootless"),
				REF("path-empty")
			)
		);
	
	pathAbEmpty_ =
		DEFINE("path-abempty",
			REPEAT(
				GLUE(
					CHAR('/'),
					REF("segment")
				)
			)
		);
	
	pathAbsolute_ =
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
	
	pathNoScheme_ =
		DEFINE("path-noscheme",
			GLUE(
				REF("segment-nz-nc"),
				REPEAT(
					GLUE(
						CHAR('/'),
						REF("segment")
					)
				)
			)
		);
	
	pathRootless_ =
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
	
	pathEmpty_ =
		DEFINE("path-empty",
			PASS()
		);
	
	segment_ =
		DEFINE("segment",
			REPEAT(
				REF("pchar")
			)
		);
	
	segmentNz_ =
		DEFINE("segment-nz",
			REPEAT(1,
				REF("pchar")
			)
		);
	
	segmentNzNc_ =
		DEFINE("segment-nz-nc",
			REPEAT(1,
				CHOICE(
					INLINE("unreserved"),
					REF("pct-encoded"),
					INLINE("sub-delims"),
					CHAR('@')
				)
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
	
	query_ =
		DEFINE("query",
			REPEAT(
				CHOICE(
					REF("pchar"),
					RANGE("/?")
				)
			)
		);
	
	fragment_ =
		DEFINE("fragment",
			REPEAT(
				CHOICE(
					REF("pchar"),
					RANGE("/?")
				)
			)
		);
	
	pctEncoded_ =
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
	
	uri_ =
		DEFINE("uri",
			GLUE(
				REPEAT(0, 1,
					GLUE(
						REF("scheme"),
						CHAR(':')
					)
				),
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
	
	ENTRY("uri");
#ifndef NDEBUG
	LINK();
#endif
}

} // namespace ftl
