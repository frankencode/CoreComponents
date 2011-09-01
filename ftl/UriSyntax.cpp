/*
 * UriSyntax.cpp -- URI syntax according to RFC3986
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "InetAddressSyntax.hpp"
#include "UriSyntax.hpp"

namespace ftl
{

UriSyntax::UriSyntax(Ref<DebugFactory> debugFactory)
	: AbnfCoreSyntax(debugFactory)
{
	SYNTAX("uri");
	IMPORT(inetAddressSyntax(), "inet");
	
	DEFINE_VOID("Delimiter",
		RANGE(":/?#[]@")
	);
	
	DEFINE_VOID("Punctuator",
		RANGE("!$&'()*+,;=")
	);
	
	/*DEFINE_VOID("Reserved",
		CHOICE(
			INLINE("Delimiter"),
			INLINE("Punctuator")
		)
	);*/
	
	DEFINE_VOID("Unreserved",
		CHOICE(
			INLINE("ALPHA"),
			INLINE("DIGIT"),
			RANGE("-._~")
		)
	);
	
	DEFINE_VOID("PathChar",
		CHOICE(
			INLINE("Unreserved"),
			INLINE("PercentEncoded"),
			INLINE("Punctuator"),
			RANGE(":@")
		)
	);
	
	DEFINE_VOID("PercentEncoded",
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
#ifndef NDEBUG
	LINK();
#endif
}

} // namespace ftl
