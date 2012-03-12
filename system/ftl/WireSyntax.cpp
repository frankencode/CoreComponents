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

	DEFINE_VOID("Identifier", REPEAT(1, EXCEPT(" \t\n:;")));
	DEFINE("Name", INLINE("Identifier"));
	DEFINE("Type", INLINE("Identifier"));

	DEFINE("Value",
		CHOICE(
			REF("AtomicValue"),
			REF("ObjectMap"),
			REF("ValueList")
		)
	);

	DEFINE("Object",
		GLUE(
			INLINE("Whitespace"),
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

	DEFINE("AtomicValue",
		REPEAT(
			GLUE(
				NOT(
					CHOICE(
						GLUE(
							INLINE("Whitespace"),
							INLINE("Name"),
							INLINE("Whitespace"),
							CHAR(':')
						),
						GLUE(
							INLINE("Whitespace"),
							RANGE(";}]")
						)
					)
				),
				ANY()
			)
		)
	);

	DEFINE("ObjectMap",
		GLUE(
			REPEAT(0, 1,
				GLUE(
					REF("Type"),
					INLINE("Whitespace")
				)
			),
			CHAR('{'),
			INLINE("Whitespace"),
			REPEAT(0, 1,
				GLUE(
					REF("Object"),
					INLINE("Whitespace"),
					REPEAT(
						GLUE(
							RANGE(";"),
							INLINE("Whitespace"),
							REF("Object"),
							INLINE("Whitespace")
						)
					)
				)
			),
			CHAR('}')
		)
	);

	DEFINE("ValueList",
		GLUE(
			CHAR('['),
			INLINE("Whitespace"),
			REPEAT(0, 1,
				GLUE(
					REF("Value"),
					INLINE("Whitespace"),
					REPEAT(
						GLUE(
							RANGE(";"),
							INLINE("Whitespace"),
							REF("Value"),
							INLINE("Whitespace")
						)
					)
				)
			),
			CHAR(']')
		)
	);

	ENTRY("Object");
}

} // namespace ftl
