 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */

#include "AbnfCoreSyntax.hpp"

namespace ftl
{

AbnfCoreSyntax::AbnfCoreSyntax(Ref<SyntaxDebugFactory> debugFactory)
	: SyntaxDefinition(debugFactory)
{
	SYNTAX("abnf-core");

	DEFINE_VOID("ALPHA",
		CHOICE(
			RANGE('a', 'z'), // 0x61 - 0x7A
			RANGE('A', 'Z')  // 0x41 - 0x5A
		)
	);

	DEFINE_VOID("DIGIT",
		RANGE('0', '9') // 0x30 - 0x39
	);

	DEFINE_VOID("HEXDIG",
		CHOICE(
			INLINE("DIGIT"),
			CHOICE(
				RANGE('a', 'f'), // 0x61 - 0x66
				RANGE('A', 'F')  // 0x41 - 0x46
			)
		)
	);

	DEFINE_VOID("BIT", RANGE("01"));  // 0x30 or 0x31
	DEFINE_VOID("WSP", RANGE(" \t")); // 0x20 or 0x09, whitespace

	DEFINE_VOID("CHAR",  RANGE(0x01, 0x7F)); // any 7-bit US-ASCII character
	DEFINE_VOID("VCHAR", RANGE(0x21, 0x7E)); // visible (printing) characters

	DEFINE_VOID("CR",     CHAR('\r')); // 0x0D
	DEFINE_VOID("LF",     CHAR('\n')); // 0x0A
	DEFINE_VOID("HTAB",   CHAR('\t')); // 0x09
	DEFINE_VOID("SP",     CHAR(' '));  // 0x20
	DEFINE_VOID("DQUOTE", CHAR('"'));  // 0x22

	DEFINE_VOID("CRLF",
		GLUE(
			REPEAT(0, 1, CHAR('\r')), // minor deviation from RFC5234
			CHAR('\n')
		)
	);

	DEFINE_VOID("CTL",
		CHOICE(
			RANGE(0x00, 0x1F),
			CHAR(0x7F)
		)
	);

	DEFINE_VOID("LWSP",
		REPEAT(
			CHOICE(
				INLINE("WSP"),
				GLUE(
					INLINE("CRLF"),
					INLINE("WSP")
				)
			)
		)
	);
}

} // namespace ftl
