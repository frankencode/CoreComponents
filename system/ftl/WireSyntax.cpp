/*
 * WireSyntax.cpp -- syntax of the Wire language
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include "WireSyntax.hpp"

namespace ftl
{

WireSyntax::WireSyntax()
{
	SYNTAX("Wire");
	
	DEFINE_VOID("Comment",
		CHOICE(
			GLUE(
				STRING("//"),
				CHOICE(
					FIND(AHEAD(CHAR('\n'))),
					FIND(EOI())
				)
			),
			GLUE(
				STRING("/*"),
				REPEAT(
					CHOICE(
						INLINE("Comment"),
						GLUE(
							NOT(STRING("*/")),
							ANY()
						)
					)
				),
				STRING("*/")
			)
		)
	);
	
	DEFINE_VOID("Whitespace",
		REPEAT(
			CHOICE(
				RANGE(" \t\n"),
				INLINE("Comment")
			)
		)
	);
	
	DEFINE("Name",
		REPEAT(1, EXCEPT(" \t\n:;"))
	);
	
	DEFINE("Value",
		CHOICE(
			REF("Atom"),
			REF("Properties"),
			REF("Items")
		)
	);
	
	DEFINE("Object",
		GLUE(
			REPEAT(0, 1,
				GLUE(
					REF("Name"),
					INLINE("Whitespace"),
					CHAR(':'),
					INLINE("Whitespace")
				)
			),
			INLINE("Value")
		)
	);
	
	DEFINE("Atom",
		REPEAT(
			GLUE(
				NOT(
					CHOICE(
						GLUE(
							REPEAT(RANGE(" \t")),
							INLINE("Name"),
							REPEAT(RANGE(" \t")),
							CHAR(':')
						),
						GLUE(
							REPEAT(RANGE(" \t")),
							RANGE("};,")
						),
						STRING("\n\n")
					)
				),
				ANY()
			)
		)
	);
	
	DEFINE("Properties",
		GLUE(
			CHAR('{'),
			INLINE("Whitespace"),
			REPEAT(
				GLUE(
					REF("Object"),
					INLINE("Whitespace"),
					RANGE(";,"),
					INLINE("Whitespace")
				)
			),
			CHAR('}')
		)
	);
	
	DEFINE("Items",
		GLUE(
			CHAR('['),
			INLINE("Whitespace"),
			REPEAT(
				GLUE(
					REF("Value"),
					INLINE("Whitespace"),
					RANGE(";,"),
					INLINE("Whitespace")
				)
			),
			CHAR(']')
		)
	);
	
	ENTRY("Object");
}

} // namespace ftl
