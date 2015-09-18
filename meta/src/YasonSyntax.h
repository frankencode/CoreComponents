/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXMETA_YASONSYNTAX_H
#define FLUXMETA_YASONSYNTAX_H

#include <flux/syntax/SyntaxDefinition>
#include <flux/meta/yason>

namespace flux { template<class> class Singleton; }

namespace flux {
namespace meta {

using namespace flux::syntax;

class YasonSyntax: public SyntaxDefinition
{
public:
    Variant parse(const ByteArray *text, const MetaProtocol *protocol = 0) const;
    Ref<MetaObject> readObject(const ByteArray *text, Token *token, const MetaProtocol *protocol = 0, MetaObject *prototype = 0) const;

    Token *nameToken(const ByteArray *text, Token *objectToken, const String &memberName) const;
    Token *valueToken(const ByteArray *text, Token *objectToken, const String &memberName) const;
    Token *childToken(Token *objectToken, int childIndex) const;

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
    int integer_;
    int float_;
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

const YasonSyntax *yasonSyntax();

}} // namespace flux::meta

#endif // FLUXMETA_YASONSYNTAX_H
