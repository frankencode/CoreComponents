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
#include "Singleton.h"
#include "Yason.h"

namespace flux
{

class YasonSyntax: public SyntaxDefinition, public Singleton<YasonSyntax>
{
public:
	Variant parseMessage(ByteArray *text, YasonProtocol *protocol = 0);
	Ref<YasonObject> parseObject(ByteArray *text, Token *token, YasonProtocol *protocol = 0, YasonObject *prototype = 0);

protected:
	friend class Singleton<YasonSyntax>;

	YasonSyntax();

	Variant parseValue(ByteArray *text, Token *token, int expectedType = Variant::UndefType, int expectedItemType = Variant::UndefType);
	Variant parseList(ByteArray *text, Token *token, int expectedItemType = Variant::UndefType);
	String parseText(ByteArray *text, Token *token);

	template<class T>
	Ref< List<T> > parseTypedList(ByteArray *text, Token *token, int expectedItemType);

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
Ref< List<T> > YasonSyntax::parseTypedList(ByteArray *text, Token *token, int expectedItemType)
{
	Ref< List<T> > list = List<T>::create(token->countChildren());
	int i = 0;
	for (Token *child = token->firstChild(); child; child = child->nextSibling()) {
		list->at(i) = parseValue(text, child, expectedItemType);
		++i;
	}
	return list;
}

inline YasonSyntax *yasonSyntax() { return YasonSyntax::instance(); }

} // namespace flux

#endif // FLUX_YASONPARSER_H
