/*
 * BlueSyntax.cpp -- Blue syntax
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include "BlueSyntax.hpp"

namespace ftl
{

BlueSyntax::BlueprintSyntaxs()
{
	SYNTAX("Blueprint");
	
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
	
	DEFINE("Key",
		GLUE(
			RANGE('a', 'z'),
			REPEAT(
				CHOICE(
					RANGE('a', 'z'),
					RANGE('A', 'Z'),
					RANGE('0', '9'),
					RANGE('_')
				)
			)
		)
	);
	
	DEFINE("Value",
		CHOICE(
			REF("Object"),
			REPEAT(
				GLUE(
					NOT(
						CHOICE(
							GLUE(
								REPEAT(RANGE(" \t")),
								INLINE("Key"),
								REPEAT(RANGE(" \t")),
								CHAR(':')
							),
							GLUE(
								REPEAT(RANGE(" \t")),
								CHAR('}')
							),
							CHAR(';'),
							STRING("\n\n")
						)
					),
					ANY()
				)
			)
		)
	);
	
	DEFINE("Type",
		GLUE(
			RANGE('A', 'Z'),
			REPEAT(
				CHOICE(
					RANGE('a', 'z'),
					RANGE('A', 'Z'),
					RANGE('0', '9'),
					RANGE('_')
				)
			)
		)
	);
	
	DEFINE("Object",
		GLUE(
			INLINE("Whitespace"),
			REF("Type"),
			INLINE("Whitespace"),
			CHAR('{'),
			INLINE("Whitespace"),
			REPEAT(
				GLUE(
					REF("Key"),
					REPEAT(RANGE(" \t")),
					CHAR(':')
					INLINE("Whitespace"),
					REF("Value"),
					INLINE("Whitespace")
				)
			),
			CHAR('}'),
			INLINE("Whitespace")
		)
	);
	
	ENTRY("Object");
}

} // namespace ftl
