/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/stdio.h> // DEBUG
#include <flux/YasonSyntax.h>
#include <flux/DebugTokenFactory.h>
#include "Markup.h"

namespace fluxdoc
{

Markup::Markup()
{
	SYNTAX("markup");

	IMPORT(yasonSyntax());

	DEFINE("Gap", REPEAT(RANGE(" \t")));

	DEFINE("EscapedChar",
		GLUE(
			CHAR('\\'),
			EXPECT("illegal escape sequence",
				CHOICE(
					RANGE("\\@/!*"),
					GLUE(
						CHAR('u'),
						REPEAT(4, 4,
							CHOICE(
								RANGE('0', '9'),
								RANGE('a', 'f'),
								RANGE('A', 'F')
							)
						)
					)
				)
			)
		)
	);

	DEFINE("LineEnd",
		GLUE(
			INLINE("Gap"),
			CHOICE(
				CHAR('\n'),
				EOI()
			)
		)
	);

	DEFINE("Line",
		REPEAT(
			GLUE(
				NOT(INLINE("LineEnd")),
				CHOICE(
					REF("yason::CommentText"),
					REF("yason::CommentLine"),
					REF("EscapedChar"),
					GLUE(
						CHAR('@'),
						EXPECT("expected YASON object",
							REF("yason::Object")
						)
					),
					ANY()
				)
			)
		)
	);

	DEFINE("Chunk",
		REPEAT(
			GLUE(
				INLINE("Gap"),
				NOT(RANGE("!*")),
				LENGTH(1, REF("Line")),
				INLINE("LineEnd")
			)
		)
	);

	DEFINE("Heading",
		GLUE(
			INLINE("Gap"),
			CHAR('!'),
			INLINE("Gap"),
			REF("Line"),
			INLINE("LineEnd")
		)
	);

	DEFINE("Item",
		GLUE(
			REF("Gap"),
			CHAR('*'),
			INLINE("Gap"),
			REF("Chunk")
		)
	);

	DEFINE("Part",
		REPEAT(
			GLUE(
				NOT(EOI()),
				CHOICE(
					REF("Item"),
					REF("Heading"),
					GLUE(
						INLINE("Gap"),
						REF("Line"),
						INLINE("LineEnd")
					)
				)
			)
		)
	);

	ENTRY("Part");
	LINK();
}

Ref<FragmentList> Markup::parse(ByteArray *text) const
{
	Ref<DebugTokenFactory> tokenFactory = DebugTokenFactory::create();
	Ref<SyntaxState> state = match(text, 0, tokenFactory);
	if (!state->valid()) throw SyntaxError(text, state);
	cast<DebugToken>(state->rootToken())->printTo(err(), text);
	return Ref<FragmentList>();
}

} // namespace fluxdoc
