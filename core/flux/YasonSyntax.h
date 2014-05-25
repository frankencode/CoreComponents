/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_YASONPARSER_H
#define FLUX_YASONPARSER_H

#include "SyntaxDefinition.h"
#include "yason.h"

namespace flux
{

template<class SubClass>
class Singleton;

class YasonSyntax: public SyntaxDefinition
{
public:
	Variant parse(const ByteArray *text, YasonProtocol *protocol = 0) const;
	Ref<YasonObject> readObject(const ByteArray *text, Token *token, YasonProtocol *protocol = 0, YasonObject *prototype = 0) const;

	Token *nameToken(const ByteArray *text, Token *objectToken, const String &memberName) const;
	Token *valueToken(const ByteArray *text, Token *objectToken, const String &memberName) const;

protected:
	friend class Singleton<YasonSyntax>;

	enum Options {
		GenerateComments     = 1,
		GenerateEscapedChars = 2
	};
	YasonSyntax(int options = 0);

	String readName(const ByteArray *text, Token *token) const;
	Variant readValue(const ByteArray *text, Token *token, int expectedType = Variant::UndefType, int expectedItemType = Variant::UndefType) const;
	Variant readList(const ByteArray *text, Token *token, int expectedItemType = Variant::UndefType) const;
	String readText(const ByteArray *text, Token *token) const;

	template<class T>
	Ref< List<T> > parseTypedList(const ByteArray *text, Token *token, int expectedItemType) const;

	int item_;
	int line_;
	int string_;
	int document_;
	int text_;
	int boolean_;
	int true_;
	int false_;
	int name_;
	int className_;
	int object_;
	int list_;
	int message_;
};

template<class T>
Ref< List<T> > YasonSyntax::parseTypedList(const ByteArray *text, Token *token, int expectedItemType) const
{
	Ref< List<T> > list = List<T>::create(token->countChildren());
	int i = 0;
	for (Token *child = token->firstChild(); child; child = child->nextSibling()) {
		list->at(i) = readValue(text, child, expectedItemType);
		++i;
	}
	return list;
}

YasonSyntax *yasonSyntax();

} // namespace flux

#endif // FLUX_YASONPARSER_H
