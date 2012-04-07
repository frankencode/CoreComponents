#include "FloatLiteral.hpp"
#include "IntegerLiteral.hpp"
#include "Wire.hpp"

namespace ftl
{

Wire::Wire()
{
	SYNTAX("Wire");

	IMPORT(floatLiteral(), "float");
	IMPORT(integerLiteral(), "int");

	DEFINE_VOID("CommentLine",
		GLUE(
			STRING("//"),
			CHOICE(
				FIND(AHEAD(CHAR('\n'))),
				FIND(EOI())
			)
		)
	);

	DEFINE_VOID("CommentText",
		GLUE(
			STRING("/*"),
			REPEAT(
				CHOICE(
					INLINE("CommentText"),
					GLUE(
						NOT(STRING("*/")),
						ANY()
					)
				)
			),
			STRING("*/")
		)
	);

	DEFINE_VOID("EscapedChar",
		GLUE(
			CHAR('\\'),
			CHOICE(
				RANGE("\"\\/bfnrt"),
				GLUE(
					CHAR('u'),
					REPEAT(4, 4, INLINE("HEXDIG"))
				)
			)
		)
	);

	DEFINE_VOID("Noise",
		REPEAT(
			CHOICE(
				RANGE(" \t\n\r"),
				INLINE("CommentLine"),
				INLINE("CommentText")
			)
		)
	);

	char_ =
		DEFINE("Char",
			GLUE(
				CHAR('\''),
				CHOICE(
					INLINE("EscapedChar"),
					OTHER('\'')
				),
				CHAR('\'')
			)
		);

	string_ =
		DEFINE("String",
			GLUE(
				CHAR('"'),
				REPEAT(
					CHOICE(
						INLINE("EscapedChar"),
						EXCEPT("\"\n")
					)
				),
				CHAR('"')
			)
		);

	concatenation_ =
		DEFINE("Concatenation",
			REPEAT(1,
				GLUE(
					REF("String"),
					INLINE("NOISE")
				)
			)
		);

	DEFINE_VOID("Identifier",
		REPEAT(1,
			GLUE(
				CHOICE(
					RANGE('a', 'z'),
					RANGE('A', 'Z'),
					CHAR('_'),
					RANGE('0', '9'),
				)
			)
		)
	);

	name_ =
		DEFINE("Name",
			CHOICE(
				GLUE(
					NOT(RANGE('0', '9')),
					INLINE("Identifier")
				),
				INLINE("String")
			)
		);

	class_ =
		DEFINE("Class",
			GLUE(
				AHEAD(RANGE('A', 'Z')),
				INLINE("Identifier")
			)
		);

	atomic_ =
		DEFINE("Atomic",
			KEYWORD("int float char string")
		);

	type_ =
		DEFINE("Type",
			CHOICE(
				INLINE("Class"),
				INLINE("Atomic")
			)
		);

	DEFINE_VOID("Value",
		CHOICE(
			REF("float::Literal"),
			REF("int::Literal"),
			REF("Char"),
			REF("String"),
			REF("Object"),
			REF("Array"),
			REF("Declaration")
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

	array_ =
		DEFINE("Array",
			GLUE(
				REPEAT(0, 1,
					GLUE(
						REF("Type"),
						INLINE("Noise")
					)
				),
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

	object_ =
		DEFINE("Object",
			GLUE(
				REPEAT(0, 1,
					GLUE(
						REF("Class"),
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

	message_ =
		DEFINE("Message",
			GLUE(
				INLINE("Noise"),
				REF("Object"),
				INLINE("Noise")
			)
		);

	ENTRY("Message");
	LINK();
}

} // namespace ftl
