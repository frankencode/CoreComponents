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

	DEFINE_VOID("Gap",
		REPEAT(1,
			CHOICE(
				GLUE(
					REPEAT(1, RANGE(" \t\n\r")),
					REPEAT(INLINE("CommentLine"))
				),
				INLINE("CommentText")
			)
		)
	);

	DEFINE_VOID("Break",
		GLUE(
			REPEAT(
				CHOICE(
					GLUE(
						REPEAT(1, RANGE(" \t")),
						REPEAT(INLINE("CommentLine"))
					),
					INLINE("CommentText")
				)
			),
			RANGE("\n;\r")
		)
	);

	DEFINE_VOID("Noise",
		REPEAT(0, 1, INLINE("Gap"))
	);

	name_ =
		DEFINE("Name",
			REPEAT(1,
				GLUE(
					NOT(RANGE('0', '9')),
					CHOICE(
						RANGE('a', 'z'),
						RANGE('A', 'Z'),
						CHAR('_'),
						RANGE('0', '9'),
						GREATER(0x7F)
					)
				)
			)
		);

	type_ =
		DEFINE("Type", INLINE("Name"));

	atom_ =
		DEFINE("Atom",
			LENGTH(1,
				CHOICE(
					CONTEXT("Member",
						FIND(
							CHOICE(
								AHEAD(
									GLUE(
										INLINE("Break"),
										INLINE("Noise"),
										INLINE("Name"),
										INLINE("Noise"),
										CHAR(':')
									)
								),
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
								GLUE(
									INLINE("Noise"),
									RANGE(",]")
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
			REF("Object"),
			REF("Array"),
			REF("Atom")
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
	LINK();
}


Ref<Node, Owner> Wire::parse(Ref<ByteArray> source)
{
	int i0 = 0, i1 = 0;
	Ref<Token, Owner> token = match(source, i0, &i1);
	FTL_CHECK(token, WireException, "Invalid syntax");
	FTL_CHECK(i1 == source->size(), WireException, "Invalid syntax");
	return parseObject(source, token->firstChild());
}

Ref<Node, Owner> Wire::parseObject(Ref<ByteArray> source, Ref<Token> token)
{
	FTL_CHECK(token->rule() == object_, WireException, "");
	String type;
	Ref<Token> child = token->firstChild();
	if (child) {
		if (child->rule() == type_) {
			type = source->copy(child->i0(), child->i1());
			child = child->nextSibling();
		}
	}
	Ref<ObjectNode, Owner> objectNode = new ObjectNode(type, source, token);
	Ref<Token> member = child;
	while (member) {
		FTL_CHECK(member->rule() == member_, WireException, "");
		Ref<Token> name = member->firstChild();
		Ref<Token> value = member->lastChild();
		objectNode->insertMember(
			source->copy(name->i0(), name->i1()),
			parseValue(source, value)
		);
		member = member->nextSibling();
	}
	return objectNode;
}

Ref<Node, Owner> Wire::parseValue(Ref<ByteArray> source, Ref<Token> token)
{
	if (token->rule() == atom_) return new Node(source, token);
	else if (token->rule() == object_) return parseObject(source, token);
	else if (token->rule() == array_) return parseArray(source, token);
	FTL_CHECK(false, WireException, "");
}

Ref<Node, Owner> Wire::parseArray(Ref<ByteArray> source, Ref<Token> token)
{
	FTL_CHECK(token->rule() == array_, WireException, "");
	Ref<ArrayNode, Owner> arrayNode = new ArrayNode(token->countChildren(), source, token);
	Ref<Token> value = token->firstChild();
	for (int i = 0, n = arrayNode->itemCount(); i < n; ++i) {
		arrayNode->setItem(i, parseValue(source, value));
		value = value->nextSibling();
	}
	return arrayNode;
}

} // namespace ftl
