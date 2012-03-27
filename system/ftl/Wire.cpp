/*
 * Wire.cpp -- a Wire language parser
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include "Wire.hpp"

namespace ftl
{

Wire::Wire()
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

	DEFINE_VOID("Gap",
		REPEAT(1,
			CHOICE(
				RANGE(" \t\n\r"),
				INLINE("Comment")
			)
		)
	);

	DEFINE_VOID("Noise",
		REPEAT(0, 1, INLINE("Gap"))
	);

	DEFINE_VOID("Break",
		GLUE(
			REPEAT(
				CHOICE(
					RANGE(" \t"),
					INLINE("Comment")
				)
			),
			RANGE("\n;\r")
		)
	);

	name_ =
		DEFINE("Name",
			GLUE(
				NOT(RANGE('0', '9')),
				CHOICE(
					RANGE('a', 'z'),
					RANGE('A', 'Z'),
					CHAR('_'),
					RANGE('0', '9'),
					GREATER(0x79)
				)
			)
		);

	type_ =
		DEFINE("Type", INLINE("Identifier"));

	atom_ =
		DEFINE("Atom",
			LENGTH(1,
				CHOICE(
					CONTEXT("Member",
						CHOICE(
							FIND(
								AHEAD(
									GLUE(
										INLINE("Break"),
										INLINE("Noise"),
										INLINE("Name"),
										INLINE("Noise"),
										CHAR(':')
									)
								)
							),
							FIND(
								AHEAD(
									GLUE(
										INLINE("Noise"),
										CHAR('}')
									)
								)
							)
						)
					),
					CONTEXT("Array",
						FIND(
							AHEAD(
								CHOICE(
									GLUE(
										INLINE("Noise"),
										RANGE(",]")
									),
									INLINE("Gap")
								)
							)
						)
					)
				)
			)
		);

	array_ =
		DEFINE("Array",
			GLUE(
				CHAR('['),
				INLINE("Noise"),
				REPEAT(0, 1,
					GLUE(
						INLINE("Value"),
						INLINE("Noise"),
						REPEAT(
							GLUE(
								REPEAT(0, 1,
									GLUE(
										CHAR(','),
										INLINE("Noise")
									)
								),
								INLINE("Value"),
								INLINE("Noise")
							)
						)
					)
				),
				CHAR(']')
			)
		);

	DEFINE_VOID("Value",
		CHOICE(
			REF("Atom"),
			REF("Object"),
			REF("Array")
		)
	);

	member_ =
		DEFINE("Member",
			GLUE(
				REF("Name"),
				INLINE("Noise"),
				CHAR(':'),
				INLINE("Noise"),
				INLINE("Value")
			)
		);

	object_ =
		DEFINE("Object",
			GLUE(
				REPEAT(0, 1,
					GLUE(
						REF("Type"),
						INLINE("Noise")
					)
				),
				CHAR('{'),
				INLINE("Noise"),
				REPEAT(0, 1,
					GLUE(
						REF("Member"),
						INLINE("Noise"),
						REPEAT(
							GLUE(
								REPEAT(0, 1, CHAR(';')),
								INLINE("Noise"),
								REF("Member"),
								INLINE("Noise")
							)
						)
					)
				),
				CHAR('}')
			)
		);

	source_ =
		DEFINE("Source",
			GLUE(
				INLINE("Noise"),
				REF("Object"),
				INLINE("Noise")
			)
		);

	ENTRY("Source");
}

} // namespace ftl
