/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/stdio.h> // DEBUG
#include <flux/DebugTokenFactory.h> // DEBUG
#include <flux/assert.h>
#include <flux/Singleton.h>
#include <flux/YasonSyntax.h>
#include "MarkupProtocol.h"
#include "elements.h"
#include "MarkupSyntax.h"

namespace fluxdoc
{

MarkupSyntax::MarkupSyntax()
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

	DEFINE("ItemText",
		REPEAT(1,
			GLUE(
				INLINE("Gap"),
				NOT(RANGE("!*@")),
				REF("Line"),
				INLINE("LineEnd")
			)
		)
	);

	DEFINE("Item", &item_,
		GLUE(
			REPLAY("indent"),
			CHAR('*'),
			REF("ItemText"),
			REPEAT(0, 1,
				GLUE(
					AHEAD(
						GLUE(
							REPLAY("indent"),
							LENGTH(1, INLINE("Gap"))
						)
					),
					INVOKE("List")
				)
			)
		)
	);

	DEFINE("List", &list_,
		GLUE(
			AHEAD(
				CAPTURE("indent",
					INLINE("Gap")
				)
			),
			REPEAT(1, REF("Item"))
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
						REF("List"),
						REF("Paragraph"),
						GLUE(
							INLINE("Gap"),
							CHAR('\n')
						)
					)
				)
			),
			EXPECT("Unrecognised symbol",
				EOI()
			)
		)
	);

	ENTRY("Part");
	LINK();
}

Ref<ElementList> MarkupSyntax::parse(ByteArray *text, String resource) const
{
	Ref<SyntaxState> state = match(text, 0, DebugTokenFactory::create());
	if (!state->valid()) throw SyntaxError(text, state, resource);

	cast<DebugToken>(state->rootToken())->printTo(err(), text);

	Ref<ElementList> elements;
	try {
		elements = readPart(text, state->rootToken());
	}
	catch (SemanticError &error) {
		error.setResource(resource);
		throw error;
	}
	return elements;
}

Ref<ElementList> MarkupSyntax::readPart(ByteArray *text, Token *partToken) const
{
	Ref<ElementList> elements = ElementList::create();

	for (
		Token *token = partToken->firstChild();
		token;
		token = token->nextSibling())
	{
		Ref<MetaObject> element;
		if (token->rule() == object_) {
			element = yasonSyntax()->readObject(text, token->firstChild(), markupProtocol());
		}
		else if (token->rule() == paragraph_) {
			String s = readLines(text, token->firstChild());
			if (s == "") continue;
			Ref<ParagraphElement> paragraph = ParagraphElement::create();
			paragraph->insert("text", s);
			paragraph->realize(text, token);
			element = paragraph;
		}
		else if (token->rule() == list_) {
			element = readList(text, token);
		}
		FLUX_ASSERT(element);
		if (element) elements->append(element);
	}

	return elements;
}

Ref<Element> MarkupSyntax::readList(ByteArray *text, Token *listToken) const
{
	Ref<ListElement> listElement = ListElement::create();
	for (Token *token = listToken->firstChild(); token; token = token->nextSibling())
		listElement->children()->append(readItem(text, token));
	listElement->realize(text, listToken);
	return listElement;
}

Ref<Element> MarkupSyntax::readItem(ByteArray *text, Token *itemToken) const
{
	Ref<ItemElement> item = ItemElement::create();
	Token *token = itemToken->firstChild();
	item->insert("text", readLines(text, token->firstChild()));
	token = token->nextSibling();
	if (token)
		item->children()->append(readList(text, token));
	item->realize(text, itemToken);
	return item;
}

String MarkupSyntax::readLines(ByteArray *text, Token *lineToken) const
{
	Ref<StringList> lines = StringList::create();
	for (; lineToken; lineToken = lineToken->nextSibling())
		lines->append(readChunks(text, lineToken->firstChild()));
	return lines->join(" ")->simplifyInsitu(" \t");
}

String MarkupSyntax::readChunks(ByteArray *text, Token *chunkToken) const
{
	Ref<StringList> chunks = StringList::create();
	for (; chunkToken; chunkToken = chunkToken->nextSibling())
		chunks->append(text->copy(chunkToken));
	return chunks->join();
}

const MarkupSyntax *markupSyntax() { return Singleton<MarkupSyntax>::instance(); }

} // namespace fluxdoc
