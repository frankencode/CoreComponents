/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "Format.h"
#include "FloatLiteral.h"
#include "IntegerLiteral.h"
#include "YasonSyntax.h"

namespace flux
{

YasonSyntax::YasonSyntax()
{
	SYNTAX("yason");

	IMPORT(floatLiteral(), "float");
	IMPORT(integerLiteral(), "int");

	DEFINE("CommentLine",
		GLUE(
			STRING("//"),
			CHOICE(
				FIND(AHEAD(CHAR('\n'))),
				FIND(EOI())
			)
		)
	);

	DEFINE("CommentText",
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
			EXPECT("missing closing '*/'",
				STRING("*/")
			)
		)
	);

	DEFINE("Noise",
		REPEAT(
			CHOICE(
				RANGE(" \t\n\r"),
				INLINE("CommentLine"),
				INLINE("CommentText")
			)
		)
	);

	DEFINE("EscapedChar",
		GLUE(
			CHAR('\\'),
			EXPECT("illegal escape sequence",
				CHOICE(
					RANGE("\"\\/fnrt"),
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

	item_ =
		DEFINE("Item",
			REPEAT(1,
				GLUE(
					NOT(
						GLUE(
							INLINE("Noise"),
							RANGE(",]")
						)
					),
					EXCEPT(",;{}:\"\n\r")
				)
			)
		);

	line_ =
		DEFINE("Line",
			GLUE(
				NOT(RANGE("[{")),
				LENGTH(1,
					FIND(
						AHEAD(
							GLUE(
								REPEAT(
									CHOICE(
										RANGE(" \t\r}"),
										INLINE("CommentLine"),
										INLINE("CommentText")
									)
								),
								CHOICE(
									AHEAD(CHAR('\n')),
									EOI()
								)
							)
						)
					)
				)
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
				EXPECT("missing closing '\"'",
					CHAR('"')
				)
			)
		);

	document_ =
		DEFINE("Document",
			FIND(AHEAD(REPLAY("eod")))
		);

	text_ =
		DEFINE("Text",
			CHOICE(
				GLUE(
					REF("String"),
					REPEAT(
						GLUE(
							INLINE("Noise"),
							REF("String")
						)
					)
				),
				GLUE(
					STRING("<<"),
					CAPTURE("eod", INLINE("Identifier")),
					REPEAT(RANGE(" \t")),
					REPEAT(0, 1, INLINE("CommentLine")),
					CHAR('\n'),
					REF("Document"),
					REPLAY("eod")
				),
				CONTEXT("List",
					REF("Item"),
					REF("Line")
				)
			)
		);

	boolean_ =
		DEFINE("Boolean",
			KEYWORD("true false")
		);

	true_ = keywordByName("true");
	false_ = keywordByName("false");

	DEFINE("Identifier",
		REPEAT(1,
			CHOICE(
				RANGE('a', 'z'),
				RANGE('A', 'Z'),
				RANGE("_-"),
				RANGE('0', '9')
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

	className_ =
		DEFINE("Class",
			GLUE(
				AHEAD(RANGE('A', 'Z')),
				INLINE("Identifier")
			)
		);

	DEFINE("Member",
		CHOICE(
			REF("Object"),
			GLUE(
				REF("Name"),
				INLINE("Noise"),
				EXPECT("expected ':'",
					CHAR(':')
				),
				INLINE("Noise"),
				EXPECT("expected value",
					INLINE("Value")
				)
			)
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
						INLINE("Member"),
						REPEAT(
							GLUE(
								LENGTH(1,
									GLUE(
										INLINE("Noise"),
										REPEAT(0, 1,
											GLUE(
												RANGE(",;"),
												INLINE("Noise")
											)
										)
									)
								),
								INLINE("Member")
							)
						)
					)
				),
				EXPECT("missing closing '}'",
					GLUE(
						INLINE("Noise"),
						CHAR('}')
					)
				)
			)
		);

	list_ =
		DEFINE("List",
			GLUE(
				CHAR('['),
				INLINE("Noise"),
				REPEAT(0, 1,
					GLUE(
						INLINE("Value"),
						REPEAT(
							GLUE(
								INLINE("Noise"),
								RANGE(",;"),
								INLINE("Noise"),
								INLINE("Value")
							)
						)
					)
				),
				EXPECT("missing closing ']'",
					GLUE(
						INLINE("Noise"),
						CHAR(']')
					)
				)
			)
		);

	DEFINE("Value",
		CHOICE(
			REF("int::Literal"),
			REF("float::Literal"),
			REF("Object"),
			REF("List"),
			REF("Boolean"),
			REF("Text")
		)
	);

	message_ =
		DEFINE("Message",
			GLUE(
				INLINE("Noise"),
				INLINE("Value"),
				INLINE("Noise")
			)
		);

	ENTRY("Message");
	LINK();
}

Variant YasonSyntax::parse(const ByteArray *text, YasonProtocol *protocol)
{
	Ref<SyntaxState> state = match(text);
	if (!state->valid()) throw SyntaxError(text, state);
	Token *valueToken = state->rootToken()->firstChild();
	if (protocol) return readObject(text, valueToken, protocol);
	return readValue(text, valueToken);
}

Ref<YasonObject> YasonSyntax::readObject(const ByteArray *text, Token *token, YasonProtocol *protocol, YasonObject *prototype)
{
	if (token->rule() != object_)
		throw SemanticError("Expected an object value", text, token->i0());

	Token *objectToken = token;
	token = token->firstChild();

	String className;
	if (token->rule() == className_)
		className = text->copy(token->i0(), token->i1());

	if (protocol) {
		if (!protocol->lookup(className, &prototype)) {
			throw SemanticError(
				Format("Object of class \"%%\" is not allowed here") << className,
				text, token->i1()
			);
		}
	}
	else if (prototype) {
		if (className != prototype->className()) {
			throw SemanticError(
				Format("Expected an object of class \"%%\"") << prototype->className_,
				text, token->i1()
			);
		}
	}

	if (token->rule() == className_)
		token = token->nextSibling();

	Ref<YasonObject> object;
	if (prototype) {
		if (protocol) object = protocol->produce(prototype);
		else object = prototype->produce();
	}
	else object = YasonObject::create(className);

	while (token) {
		if (token->rule() == name_) {
			bool stripQuotation = (text->at(token->i0()) == '"');
			String name = text->copy(token->i0() + stripQuotation, token->i1() - stripQuotation);

			Variant defaultValue;
			YasonObject *memberPrototype = 0;
			if (prototype) {
				if (!prototype->lookup(name, &defaultValue))
					throw SemanticError(
						Format("Member \"%%\" is not supported") << name,
						text, token->i1()
					);
				if (type(defaultValue) == Variant::ObjectType)
					memberPrototype = cast<YasonObject>(defaultValue);
			}

			token = token->nextSibling();

			Variant value;
			if (memberPrototype)
				value = readObject(text, token, 0, memberPrototype);
			else
				value = readValue(text, token, type(defaultValue), itemType(defaultValue));

			Variant existingValue;
			if (object->lookup(name, &existingValue)) {
				if (value != existingValue) {
					throw SemanticError(
						Format("Ambiguous value for member \"%%\"") << name,
						text, token->i1()
					);
				}
			}
			object->insert(name, value);

			object->realizeMember(name, value, text, token->previousSibling(), token);
		}
		else {
			YasonProtocol *prototypeProtocol = 0;
			if (prototype) prototypeProtocol = prototype->protocol();
			Ref<YasonObject> child = readObject(text, token, prototypeProtocol);
			object->children()->append(child);
		}
		token = token->nextSibling();
	}

	if (prototype) {
		if (object->size() != prototype->size()) {
			for (int i = 0; i < prototype->size(); ++i) {
				String name = prototype->keyAt(i);
				if (object->size() <= i || object->keyAt(i) != name)
					object->insert(name, prototype->valueAt(i));
			}
		}
	}

	object->realize(objectToken);

	return object;
}

Variant YasonSyntax::readValue(const ByteArray *text, Token *token, int expectedType, int expectedItemType)
{
	Variant value;
	bool typeError = false;

	if (token->scope() == floatLiteral()->id())
	{
		if ( expectedType == Variant::UndefType ||
		     expectedType == Variant::FloatType ||
		     expectedItemType == Variant::FloatType )
		{
			float64_t x;
			floatLiteral()->read(&x, text, token);
			value = x;
		}
		else
			typeError = true;
	}
	else if (token->scope() == integerLiteral()->id())
	{
		if ( expectedType == Variant::UndefType ||
		     expectedType == Variant::FloatType ||
		     expectedType == Variant::IntType ||
		     expectedItemType == Variant::FloatType ||
		     expectedItemType == Variant::IntType )
		{
			uint64_t x; int s;
			integerLiteral()->read(&x, &s, text, token);
			value = int(x) * s;
			if (expectedType == Variant::FloatType)
				value = Variant::real(value);
		}
		else
			typeError = true;
	}
	else if (token->rule() == text_)
	{
		if ( expectedType == Variant::UndefType ||
		     expectedType == Variant::StringType ||
		     expectedItemType == Variant::StringType )
			value = readText(text, token);
		else
			typeError = true;
	}
	else if (token->rule() == object_)
	{
		value = readObject(text, token);
	}
	else if (token->rule() == list_)
	{
		if ( expectedType == Variant::UndefType ||
		     expectedType == Variant::ListType )
			value = readList(text, token, expectedItemType);
		else
			typeError = true;
	}
	else if (token->rule() == boolean_)
	{
		if ( expectedType == Variant::UndefType ||
		     expectedType == Variant::BoolType ||
		     expectedItemType == Variant::BoolType )
		{
			if (token->keyword() == true_)
				value = true;
			else if (token->keyword() == false_)
				value = false;
		}
		else
			typeError = true;
	}

	if (typeError) {
		throw SemanticError(
			Format("Expected a value of type %%") << Variant::typeName(expectedType, expectedItemType),
			text, token->i0()
		);
	}

	if (expectedType == Variant::ListType && type(value) != Variant::ListType) {
		if (expectedItemType == Variant::IntType) {
			Ref< List<int> > list = List<int>::create();
			list->append(value);
			value = list;
		}
		else if (expectedItemType == Variant::BoolType) {
			Ref< List<bool> > list = List<bool>::create();
			list->append(value);
			value = list;
		}
		else if (expectedItemType == Variant::FloatType) {
			Ref< List<float> > list = List<float>::create();
			list->append(value);
			value = list;
		}
		else if (expectedItemType == Variant::StringType) {
			Ref< List<String> > list = List<String>::create();
			list->append(value);
			value = list;
		}
		else {
			Ref<VariantList> list = VariantList::create();
			list->append(value);
			value = list;
		}
	}

	return value;
}

Variant YasonSyntax::readList(const ByteArray *text, Token *token, int expectedItemType)
{
	Variant list;
	if (expectedItemType == Variant::IntType)
		list = parseTypedList<int>(text, token, expectedItemType);
	else if (expectedItemType == Variant::BoolType)
		list = parseTypedList<bool>(text, token, expectedItemType);
	else if (expectedItemType == Variant::FloatType)
		list = parseTypedList<float>(text, token, expectedItemType);
	else if (expectedItemType == Variant::StringType)
		list = parseTypedList<String>(text, token, expectedItemType);
	else
		list = parseTypedList<Variant>(text, token, expectedItemType);
	return list;
}

String YasonSyntax::readText(const ByteArray *text, Token *token)
{
	token = token->firstChild();

	String s;

	if (token->rule() != string_) {
		s = text->copy(token->i0(), token->i1());
	}
	else {
		Ref<StringList> l = StringList::create();
		while (token) {
			l->append(text->copy(token->i0() + 1, token->i1() - 1));
			token = token->nextSibling();
		}
		s = (l->size() == 1) ? l->at(0) : l->join();
		s->unescapeInsitu();
	}

	return s;
}

} // namespace flux
