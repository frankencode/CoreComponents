/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/stdio.h> // DEBUG
#include <flux/assert.h>
#include <flux/YasonSyntax.h>
#include <flux/DebugTokenFactory.h>
#include "MarkupProtocol.h"
#include "fragments.h"
#include "Markup.h"

namespace fluxdoc
{

Markup::Markup()
{
	SYNTAX("markup");

	IMPORT(yasonSyntax());

	DEFINE("Gap", REPEAT(RANGE(" \t")));

	DEFINE("EscapedChar", &escapedChar_,
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

	DEFINE("Object", &object_,
		GLUE(
			REPEAT(
				CHOICE(
					LENGTH(1, INLINE("Gap")),
					INLINE("yason::CommentText")
				)
			),
			CHAR('@'),
			EXPECT("expected YASON object",
				REF("yason::Object")
			),
			REPEAT(
				CHOICE(
					LENGTH(1, INLINE("Gap")),
					REF("yason::CommentText"),
					REF("yason::CommentLine")
				)
			),
			INLINE("LineEnd")
		)
	);

	DEFINE("Line", &line_,
		LENGTH(1,
			REPEAT(
				GLUE(
					NOT(INLINE("LineEnd")),
					CHOICE(
						REF("yason::CommentText"),
						REF("yason::CommentLine"),
						REF("EscapedChar"),
						OTHER('@'),
						GLUE(
							CHAR('@'),
							EXPECT("Please escape the '@' symbol", FAIL())
						)
					)
				)
			)
		)
	);

	DEFINE("Chunk", &chunk_,
		REPEAT(
			GLUE(
				INLINE("Gap"),
				NOT(RANGE("!*")),
				REF("Line"),
				INLINE("LineEnd")
			)
		)
	);

	DEFINE("Heading", &heading_,
		GLUE(
			INLINE("Gap"),
			CHAR('!'),
			INLINE("Gap"),
			REF("Line"),
			INLINE("LineEnd")
		)
	);

	DEFINE("Item", &item_,
		GLUE(
			REF("Gap"),
			CHAR('*'),
			INLINE("Gap"),
			REF("Chunk")
		)
	);

	DEFINE("Part",
		GLUE(
			REPEAT(
				GLUE(
					NOT(EOI()),
					CHOICE(
						REF("Object"),
						REF("Item"),
						REF("Heading"),
						GLUE(
							INLINE("Gap"),
							REPEAT(0, 1, REF("Line")),
							INLINE("LineEnd")
						)
					)
				)
			),
			EOI()
		)
	);

	ENTRY("Part");
	LINK();
}

Ref<FragmentList> Markup::parse(ByteArray *text, String resource) const
{
	Ref<DebugTokenFactory> tokenFactory = DebugTokenFactory::create();
	Ref<SyntaxState> state = match(text, 0, tokenFactory);
	if (!state->valid()) throw SyntaxError(text, state, resource);

	// cast<DebugToken>(state->rootToken())->printTo(err(), text);

	Ref<FragmentList> fragments;
	try {
		fragments = readPart(text, state->rootToken());
	}
	catch (SemanticError &error) {
		error.setResource(resource);
		throw error;
	}
	return fragments;
}

Ref<FragmentList> Markup::readPart(ByteArray *text, Token *partToken) const
{
	Ref<FragmentList> fragments = FragmentList::create();

	int realDepth = 0;
	int normalizedDepth = 0;

	for (
		Token *token = partToken->firstChild();
		token;
		token = token->nextSibling())
	{
		Ref<YasonObject> fragment;
		if (token->rule() == object_) {
			fragment = yasonSyntax()->readObject(text, token->firstChild(), markupProtocol());
		}
		else if (token->rule() == line_) {
			Ref<Paragraph> paragraph = Paragraph::create();
			paragraph->insert("text", text->copy(token));
			paragraph->realize(token);
			fragment = paragraph;
		}
		else if (token->rule() == heading_) {
			Ref<Heading> heading = Heading::create();
			heading->insert("text", text->copy(token->firstChild()));
			heading->realize(token);
			fragment = heading;
		}
		else if (token->rule() == item_) {
			Token *gapToken = token->firstChild();
			Token *chunkToken = gapToken->nextSibling();

			int gapSize = text->copy(gapToken)->size();
			normalizedDepth += (realDepth < gapSize) - (gapSize < realDepth);
			realDepth = gapSize;

			Ref<StringList> lines = StringList::create();
			for (
				Token *lineToken = chunkToken->firstChild();
				lineToken;
				lineToken = lineToken->nextSibling()
			)
				lines->append(text->copy(lineToken));

			Ref<Item> item = Item::create();
			item->insert("depth", normalizedDepth);
			item->insert("text", lines->join("\n"));
			item->realize(token);
			fragment = item;
		}
		if (token->rule() != item_) {
			realDepth = normalizedDepth = 0;
		}
		FLUX_ASSERT(fragment);
		if (fragment) fragments->append(fragment);
	}

	return fragments;
}

} // namespace fluxdoc
