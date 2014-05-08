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

	DEFINE("Comment", &comment_,
		CHOICE(
			INLINE("yason::CommentText"),
			INLINE("yason::CommentLine")
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

	DEFINE("Noise",
		REPEAT(
			CHOICE(
				LENGTH(1, INLINE("Gap")),
				INLINE("Comment")
			)
		)
	);

	DEFINE("Object", &object_,
		GLUE(
			INLINE("Noise"),
			CHAR('@'),
			EXPECT("expected YASON object",
				REF("yason::Object")
			),
			INLINE("Noise"),
			INLINE("LineEnd")
		)
	);

	DEFINE("Chunk", &chunk_,
		REPEAT(1,
			GLUE(
				NOT(INLINE("LineEnd")),
				NOT(INLINE("Comment")),
				CHOICE(
					REF("EscapedChar"),
					OTHER('@'),
					EXPECT("Please escape the '@' symbol", FAIL())
				)
			)
		)
	);

	DEFINE("Line", &line_,
		LENGTH(1,
			REPEAT(
				GLUE(
					NOT(INLINE("LineEnd")),
					CHOICE(
						REF("Chunk"),
						INLINE("Comment")
					)
				)
			)
		)
	);

	DEFINE("Heading", &heading_,
		GLUE(
			INLINE("Gap"),
			CHAR('!'),
			INLINE("Gap"),
			NOT(RANGE("!*@")),
			REF("Line"),
			INLINE("LineEnd")
		)
	);

	DEFINE("Item", &item_,
		GLUE(
			REF("Gap"),
			CHAR('*'),
			REPEAT(1,
				GLUE(
					INLINE("Gap"),
					NOT(RANGE("!*@")),
					REF("Line"),
					INLINE("LineEnd")
				)
			)
		)
	);

	DEFINE("ParagraphEnd",
		AHEAD(
			CHOICE(
				GLUE(
					BEHIND(
						CHAR('\n')
					),
					CHOICE(
						REPEAT(1, RANGE(" \t\n")),
						RANGE("!*@")
					)
				),
				EOI()
			)
		)
	);

	DEFINE("Paragraph", &paragraph_,
		GLUE(
			INLINE("Gap"),
			NOT(RANGE("!*@")),
			REF("Line"),
			INLINE("LineEnd"),
			REPEAT(
				GLUE(
					NOT(INLINE("ParagraphEnd")),
					INLINE("Gap"),
					NOT(RANGE("!*@")),
					REF("Line"),
					INLINE("LineEnd")
				)
			)
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
						REF("Paragraph"),
						GLUE(
							INLINE("Gap"),
							CHAR('\n')
						)
					)
				)
			),
			EXPECT("",
				EOI()
			)
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
		else if (token->rule() == paragraph_) {
			Ref<Paragraph> paragraph = Paragraph::create();
			paragraph->insert("text", readLines(text, token->firstChild()));
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
			int gapSize = gapToken->size();
			normalizedDepth += (realDepth < gapSize) - (gapSize < realDepth);
			realDepth = gapSize;

			Ref<Item> item = Item::create();
			item->insert("depth", normalizedDepth);
			item->insert("text", readLines(text, gapToken->nextSibling()));
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

String Markup::readLines(ByteArray *text, Token *lineToken) const
{
	Ref<StringList> lines = StringList::create();
	for (; lineToken; lineToken = lineToken->nextSibling())
		lines->append(readChunks(text, lineToken->firstChild()));
	return lines->join(" ")->simplifyInsitu(" \t");
}

String Markup::readChunks(ByteArray *text, Token *chunkToken) const
{
	Ref<StringList> chunks = StringList::create();
	for (; chunkToken; chunkToken = chunkToken->nextSibling())
		chunks->append(text->copy(chunkToken));
	return chunks->join();
}

} // namespace fluxdoc
