/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "Singleton.h"
#include "Format.h"
#include "FloatSyntax.h"
#include "IntegerSyntax.h"
#include "YasonSyntax.h"

namespace flux
{

YasonSyntax::YasonSyntax(int options)
{
	SYNTAX("yason");

	IMPORT(floatSyntax(), "float");
	IMPORT(integerSyntax(), "int");

	DEFINE_OPTIONAL("CommentLine", options & GenerateComments,
		GLUE(
			STRING("//"),
			CHOICE(
				FIND(AHEAD(CHAR('\n'))),
				FIND(EOI())
			)
		)
	);

	DEFINE_OPTIONAL("CommentText", options & GenerateComments,
		GLUE(
			STRING("/*"),
			REPEAT(
				CHOICE(
					REF("CommentText"),
					GLUE(
						NOT(STRING("*/")),
						ANY()
					)
				)
			),
			HINT("missing closing '*/'",
				STRING("*/")
			)
		)
	);

	DEFINE_VOID("Noise",
		REPEAT(
			CHOICE(
				RANGE(" \t\n\r"),
				REF("CommentLine"),
				REF("CommentText")
			)
		)
	);

	DEFINE_OPTIONAL("EscapedChar", options & GenerateEscapedChars,
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

	DEFINE("Item", &item_,
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

	DEFINE("Line", &line_,
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

	DEFINE("String", &string_,
		GLUE(
			CHAR('"'),
			REPEAT(
				CHOICE(
					REF("EscapedChar"),
					EXCEPT("\"\n")
				)
			),
			EXPECT("missing closing '\"'",
				CHAR('"')
			)
		)
	);

	DEFINE("Document", &document_,
		FIND(AHEAD(REPLAY("eod")))
	);

	DEFINE("Text", &text_,
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
				REPEAT(0, 1, REF("CommentLine")),
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

	DEFINE("Boolean", &boolean_,
		KEYWORD("true false")
	);

	true_ = keywordByName("true");
	false_ = keywordByName("false");

	DEFINE("Integer", &integer_,
		INLINE("int::Literal")
	);

	DEFINE("Float", &float_,
		INLINE("float::Literal")
	);

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

	DEFINE("Name", &name_,
		CHOICE(
			GLUE(
				NOT(RANGE('0', '9')),
				INLINE("Identifier")
			),
			INLINE("String")
		)
	);

	DEFINE("ClassName", &className_,
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

	DEFINE("Object", &object_,
		GLUE(
			REPEAT(0, 1,
				GLUE(
					REF("ClassName"),
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

	DEFINE("List", &list_,
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
			REF("Integer"),
			REF("Float"),
			REF("Object"),
			REF("List"),
			REF("Boolean"),
			REF("Text")
		)
	);

	DEFINE("Message", &message_,
		GLUE(
			INLINE("Noise"),
			INLINE("Value"),
			INLINE("Noise")
		)
	);

	ENTRY("Message");
	LINK();
}

Variant YasonSyntax::parse(const ByteArray *text, const MetaProtocol *protocol) const
{
	Ref<SyntaxState> state = match(text);
	if (!state->valid()) throw SyntaxError(text, state);
	Token *valueToken = state->rootToken()->firstChild();
	if (protocol) return readObject(text, valueToken, protocol);
	return readValue(text, valueToken);
}

Ref<MetaObject> YasonSyntax::readObject(const ByteArray *text, Token *token, const MetaProtocol *protocol, MetaObject *prototype) const
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
				Format("Expected an object of class \"%%\"") << prototype->className(),
				text, token->i1()
			);
		}
	}

	if (token->rule() == className_)
		token = token->nextSibling();

	Ref<MetaObject> object;
	if (prototype) {
		if (protocol) object = protocol->produce(prototype);
		else object = prototype->produce();
	}
	else object = MetaObject::create(className);

	while (token) {
		if (token->rule() == name_) {
			String name = readName(text, token);

			Variant defaultValue;
			MetaObject *memberPrototype = 0;
			if (prototype) {
				if (!prototype->lookup(name, &defaultValue))
					throw SemanticError(
						Format("Member \"%%\" is not supported") << name,
						text, token->i1()
					);
				if (type(defaultValue) == Variant::ObjectType)
					memberPrototype = cast<MetaObject>(defaultValue);
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
		}
		else {
			MetaProtocol *prototypeProtocol = 0;
			if (prototype) prototypeProtocol = prototype->protocol();
			if (prototypeProtocol) {
				if (object->children()->count() >= prototypeProtocol->maxCount()) {
					throw SemanticError(
						Format("Maximum number of children (%%) exceeded") << prototypeProtocol->maxCount(),
						text, token->i0()
					);
				}
			}
			Ref<MetaObject> child = readObject(text, token, prototypeProtocol);
			object->children()->append(child);
		}
		token = token->nextSibling();
	}

	object->autocomplete(prototype);

	if (prototype) {
		MetaProtocol *prototypeProtocol = prototype->protocol();
		if (prototypeProtocol) {
			if (prototypeProtocol->minCount() > 0) {
				if (!object->hasChildren() || object->children()->count() < prototypeProtocol->minCount()) {
					if (prototypeProtocol->prototypes_->count() == 1 && prototypeProtocol->minCount() == 1) {
						throw SemanticError(
							Format("Object of type %% needs at least one child of type %%") << object->className() << prototypeProtocol->prototypes_->at(0)->value()->className(),
							text, objectToken->i0()
						);
					}
					else {
						throw SemanticError(
							Format("Object of type %% needs at least %% children") << object->className() << prototypeProtocol->minCount(),
							text, objectToken->i0()
						);
					}
				}
			}
		}
	}

	object->realize(text, objectToken);

	return object;
}

Token *YasonSyntax::nameToken(const ByteArray *text, Token *objectToken, const String &memberName) const
{
	for (Token *token = objectToken->firstChild(); token; token = token->nextSibling()) {
		if (token->rule() == name_) {
			if (readName(text, token) == memberName)
				return token;
		}
	}
	return 0;
}

Token *YasonSyntax::valueToken(const ByteArray *text, Token *objectToken, const String &memberName) const
{
	Token *token = nameToken(text, objectToken, memberName);
	if (token) return token->nextSibling();
	return 0;
}

Token *YasonSyntax::childToken(Token *objectToken, int childIndex) const
{
	for (Token *token = objectToken->firstChild(); token; token = token->nextSibling()) {
		if (token->rule() == object_) {
			if (childIndex == 0) return token;
			--childIndex;
		}
	}
	return 0;
}

String YasonSyntax::readName(const ByteArray *text, Token *token) const
{
	bool stripQuotation = (text->at(token->i0()) == '"');
	return text->copy(token->i0() + stripQuotation, token->i1() - stripQuotation);
}

Variant YasonSyntax::readValue(const ByteArray *text, Token *token, int expectedType, int expectedItemType) const
{
	Variant value;
	bool typeError = false;

	if (token->rule() == float_)
	{
		if ( expectedType == Variant::UndefType ||
		     expectedType == Variant::FloatType ||
		     expectedItemType == Variant::FloatType )
		{
			float64_t x;
			floatSyntax()->read(&x, text, token);
			value = x;
		}
		else
			typeError = true;
	}
	else if (token->rule() == integer_)
	{
		if ( expectedType == Variant::UndefType ||
		     expectedType == Variant::FloatType ||
		     expectedType == Variant::IntType ||
		     expectedItemType == Variant::FloatType ||
		     expectedItemType == Variant::IntType )
		{
			uint64_t x; int s;
			integerSyntax()->read(&x, &s, text, token);
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
		{
			value = readText(text, token);
		}
		else if ( expectedType == Variant::ColorType ||
		          expectedItemType == Variant::ColorType )
		{
			String s = readText(text, token);
			bool ok = false;
			value = Color::parse(s, &ok);
			if (!ok) typeError = true;
		}
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

Variant YasonSyntax::readList(const ByteArray *text, Token *token, int expectedItemType) const
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

String YasonSyntax::readText(const ByteArray *text, Token *token) const
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
		s = (l->count() == 1) ? l->at(0) : l->join();
		s->unescapeInsitu();
	}

	return s;
}

const YasonSyntax *yasonSyntax() { return Singleton<YasonSyntax>::instance(); }

} // namespace flux
