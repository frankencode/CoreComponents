/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "AbnfSyntaxDefinition.h"

namespace flux {
namespace abnf {

Ref<AbnfSyntaxDefinition> AbnfSyntaxDefinition::create(SyntaxDebugFactory *debugFactory)
{
	return new AbnfSyntaxDefinition(debugFactory);
}

AbnfSyntaxDefinition::AbnfSyntaxDefinition(SyntaxDebugFactory *debugFactory)
	: SyntaxDefinition(debugFactory)
{
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

	DEFINE("BIT", RANGE("01"));  // 0x30 or 0x31
	DEFINE("WSP", RANGE(" \t")); // 0x20 or 0x09, whitespace

	DEFINE("CHAR",  RANGE(0x01, 0x7F)); // any 7-bit US-ASCII character
	DEFINE("VCHAR", RANGE(0x21, 0x7E)); // visible (printing) characters

	DEFINE("CR",     CHAR('\r')); // 0x0D
	DEFINE("LF",     CHAR('\n')); // 0x0A
	DEFINE("HTAB",   CHAR('\t')); // 0x09
	DEFINE("SP",     CHAR(' '));  // 0x20
	DEFINE("DQUOTE", CHAR('"'));  // 0x22

	DEFINE("CRLF",
		GLUE(
			REPEAT(0, 1, CHAR('\r')), // minor deviation from RFC5234
			CHAR('\n')
		)
	);

	DEFINE("CTL",
		CHOICE(
			RANGE(0x00, 0x1F),
			CHAR(0x7F)
		)
	);

	DEFINE("LWSP",
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

}} // namespace flux::abnf
