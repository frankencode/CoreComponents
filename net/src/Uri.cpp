/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/exceptions>
#include <flux/Format>
#include <flux/net/UriSyntax>
#include <flux/net/Uri>

namespace flux {
namespace net {

Uri::Uri()
    : port_(-1)
{}

Uri::Uri(const char *text)
    : port_(-1)
{
    readUri(String(text));
}

Uri::Uri(ByteArray *text, Token *rootToken)
    : port_(-1)
{
    readUri(text, rootToken);
}

void Uri::readUri(ByteArray *text, Token *rootToken)
{
    Ref<Token> rootToken2;
    if (!rootToken) {
        rootToken2 = uriSyntax()->match(text)->rootToken();
        if (!rootToken2) throw SyntaxError(text);
        rootToken = rootToken2;
    }

    Token *token = rootToken;
    token = token->firstChild();

    while (token) {
        if (token->rule() == uriSyntax()->scheme()) {
            scheme_ = decode(text->copy(token->i0(), token->i1()));
        }
        else if (token->rule() == uriSyntax()->authority()) {
            Token *child = token->firstChild();
            while (child) {
                if (child->rule() == uriSyntax()->userInfo())
                    userInfo_ = decode(text->copy(child->i0(), child->i1()));
                else if (child->rule() == uriSyntax()->host())
                    host_ = decode(text->copy(child->i0(), child->i1()));
                else if (child->rule() == uriSyntax()->port())
                    port_ = decode(text->copy(child->i0(), child->i1()))->toInt();
                child = child->nextSibling();
            }
        }
        else if (token->rule() == uriSyntax()->path()) {
            path_ = decode(text->copy(token->i0(), token->i1()));
        }
        else if (token->rule() == uriSyntax()->query()) {
            query_ = decode(text->copy(token->i0(), token->i1()));
        }
        else if (token->rule() == uriSyntax()->fragment()) {
            fragment_ = decode(text->copy(token->i0(), token->i1()));
        }
        token = token->nextSibling();
    }
}

String Uri::toString() const
{
    Format text;
    if (scheme_ != "") {
        text << encode(scheme_);
        text << ":";
    }
    if (host_ != "") {
        text << "//";
        if (userInfo_ != "")
            text << encode(userInfo_);
        if (host_ != "") {
            text << "@";
            text << encode(host_);
        }
        if (port_ != -1)
            text << ":" << port_;
    }
    text << path_;
    if (query_ != "") {
        text << "?";
        text << encode(query_);
    }
    if (fragment_ != "") {
        text << "#";
        text << encode(fragment_);
    }
    return text;
}

String Uri::encode(String s)
{
    s->downcaseInsitu();

    const char *reserved = ":/?#[]@!$&'()*+,;=";
    Ref<StringList> l = StringList::create();
    int j = 0;
    for (int i = 0, n = s->count(); i < n; ++i) {
        char ch = s->at(i);
        for (const char *r = reserved; *r; ++r) {
            if (ch == *r) {
                if (j < i)
                    l->append(s->copy(j, i));
                String pct("%XX");
                pct->at(1) = ch >> 4;
                pct->at(2) = ch & 0xF;
                l->append(pct);
                j = i + 1;
                break;
            }
        }
    }
    if (j == 0) return s;
    if (j < s->count())
        l->append(s->copy(j, s->count()));
    return l->join();
}

String Uri::decode(String s)
{
    s->downcaseInsitu();

    int j = 0;
    for (int i = 0, n = s->count(); i < n; ++i, ++j) {
        char ch = s->at(i);
        if ((ch == '%') && (i + 2 < n)) {
            unsigned char x = 0;
            char d = s->at(i + 1);
            bool match = true;
            if (('0' <= d) && (d <= '9')) x = d - '0';
            else if (('a' <= d) && (d <= 'f')) x = d - 'a' + 10;
            // else if (('A' <= d) && (d <= 'F')) x = d - 'F' + 10;
            else match = false;
            if (match) {
                x *= 16;
                d = s->at(i + 2);
                if (('0' <= d) && (d <= '9')) x += d - '0';
                else if (('a' <= d) && (d <= 'f')) x += d - 'a' + 10;
                // else if (('A' <= d) && (d <= 'F')) x = d - 'F' + 10;
                else match = false;
                if (match) {
                    i += 2;
                    ch = (char)x;
                }
            }
        }
        if (j < i) s->at(j) = ch;
    }
    s->truncate(j);
    return s;
}

}} // namespace flux::net
