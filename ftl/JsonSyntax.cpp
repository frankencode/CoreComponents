/*
 * JsonSyntax.cpp -- JSON syntax according to RFC 4627
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include "JsonSyntax.hpp"

namespace ftl
{

JsonSyntax::JsonSyntax()
{
	DEFINE_VOID("ws", REPEAT(RANGE(" \t\n\r")));
	
	DEFINE_VOID("begin-object",
		GLUE(
			INLINE("ws"),
			CHAR('{'),
			INLINE("ws")
		)
	);
	
	DEFINE_VOID("end-object",
		GLUE(
			INLINE("ws"),
			CHAR('}'),
			INLINE("ws")
		)
	);
	
	DEFINE_VOID("begin-array",
		GLUE(
			INLINE("ws"),
			CHAR('['),
			INLINE("ws")
		)
	);
	
	DEFINE_VOID("end-array",
		GLUE(
			INLINE("ws"),
			CHAR(']'),
			INLINE("ws")
		)
	);
	
	DEFINE("name-separator",
		GLUE(
			INLINE("ws"),
			CHAR(':'),
			INLINE("ws")
		)
	);
	
	DEFINE("value-separator",
		GLUE(
			INLINE("ws"),
			CHAR(','),
			INLINE("ws")
		)
	);
	
	int_ =
		DEFINE("int",
			CHOICE(
				CHAR('0'),
				GLUE(
					RANGE('1', '9'),
					REPEAT(INLINE("DIGIT"))
				)
			)
		);
	
	frac_ =
		DEFINE("frac",
			GLUE(
				CHAR('.'),
				REPEAT(1, INLINE("DIGIT"))
			)
		);
	
	exp_ =
		DEFINE("exp",
			GLUE(
				RANGE("eE"),
				REPEAT(0, 1, RANGE("+-")),
				REPEAT(1, INLINE("DIGIT"))
			)
		);
	
	number_ =
		DEFINE("number",
			GLUE(
				REPEAT(0, 1, CHAR('-')),
				REF("int"),
				REPEAT(0, 1, REF("frac")),
				REPEAT(0, 1, REF("exp"))
			)
		);
	
	string_ =
		DEFINE("string",
			GLUE(
				CHAR('"'),
				REPEAT(
					CHOICE(
						GLUE(
							CHAR('\\'),
							CHOICE(
								RANGE("\"\\/bfnrt"),
								GLUE(
									CHAR('u'),
									REPEAT(4, 4, INLINE("HEXDIG"))
								)
							)
						),
						GLUE(
							NOT(RANGE("\\\"")),
							RANGE(0x20, 0xFF)
								// minor deviation from standard to make it work on raw bytes
						)
					)
				),
				CHAR('"')
			)
		);
	
	value_ =
		DEFINE("value",
			CHOICE(
				KEYWORD("true false null"),
				REF("object"),
				REF("array"),
				REF("number"),
				REF("string")
			)
		);
	
	member_ =
		DEFINE("member",
			GLUE(
				REF("string"),
				INLINE("name-separator"),
				REF("value")
			)
		);
	
	object_ =
		DEFINE("object",
			GLUE(
				INLINE("begin-object"),
				REPEAT(0, 1,
					GLUE(
						REF("member"),
						REPEAT(
							GLUE(
								INLINE("value-separator"),
								REF("member")
							)
						)
					)
				),
				INLINE("end-object")
			)
		);
	
	array_ =
		DEFINE("array",
			GLUE(
				INLINE("begin-array"),
				REPEAT(0, 1,
					GLUE(
						REF("value"),
						REPEAT(
							GLUE(
								INLINE("value-separator"),
								REF("value")
							)
						)
					)
				),
				INLINE("end-array")
			)
		);
	
	text_ =	
		DEFINE("JSON-text",
			CHOICE(
				REF("object"),
				REF("array")
			)
		);
	
	ENTRY("JSON-text");
#ifndef NDEBUG
	LINK();
#endif
}

} // namespace ftl
