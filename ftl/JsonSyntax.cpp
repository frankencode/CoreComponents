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
	
	member_ =
		DEFINE("member",
			GLUE(
				REF("string"),
				INLINE("name-separator"),
				REF("value")
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
	
	int_ =
		DEFINE("int",
			CHOICE(
				CHAR('0'),
				GLUE(
					RANGE('1', '9'),
					REPEAT(RANGE('0', '9'))
				)
			)
		);
	
	frac_ =
		DEFINE("frac",
			GLUE(
				CHAR('.'),
				REPEAT(1, RANGE('0', '9'))
			)
		);
	
	exp_ =
		DEFINE("exp",
			GLUE(
				RANGE("eE"),
				REPEAT(0, 1, RANGE("+-")),
				REPEAT(1, RANGE('0', '9'))
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
	
	DEFINE("JSON-text",
		CHOICE(
			REF("object"),
			REF("array")
		)
	);
	
	ENTRY("JSON-text");
	LINK();
}

} // namespace ftl
