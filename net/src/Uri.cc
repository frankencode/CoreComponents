/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Format>
#include <cc/syntax/Token>
#include <cc/net/SocketAddress>
#include <cc/net/UriSyntax>
#include <cc/net/Uri>

namespace cc {
namespace net {

using namespace cc::syntax;

Ref<Uri> Uri::create()
{
    return new Uri;
}

Ref<Uri> Uri::parse(const String &text, Token *rootToken)
{
    return new Uri{text, rootToken};
}

Uri::Uri():
    hostIsNumeric_{false},
    port_{-1}
{}

Uri::Uri(const CharArray *text, Token *rootToken):
    hostIsNumeric_{false},
    port_{-1}
{
    readUri(text, rootToken);
}

void Uri::readUri(const CharArray *text, Token *rootToken)
{
    Ref<Token> rootToken2;
    if (!rootToken) {
        rootToken2 = UriSyntax::instance()->match(text)->rootToken();
        if (!rootToken2) throw UriSyntaxError(text);
        rootToken = rootToken2;
    }

    Token *token = rootToken;
    token = token->firstChild();

    while (token) {
        if (token->rule() == UriSyntax::instance()->scheme()) {
            scheme_ = decode(text->copy(token->i0(), token->i1()));
            mutate(scheme_)->downcaseInsitu();
        }
        else if (token->rule() == UriSyntax::instance()->authority()) {
            Token *child = token->firstChild();
            while (child) {
                if (child->rule() == UriSyntax::instance()->userInfo())
                    userInfo_ = decode(text->copy(child->i0(), child->i1()));
                else if (child->rule() == UriSyntax::instance()->host()) {
                    hostIsNumeric_ = child->hasChildren();
                    host_ = decode(text->copy(child->i0(), child->i1()));
                }
                else if (child->rule() == UriSyntax::instance()->port())
                    port_ = decode(text->copy(child->i0(), child->i1()))->toNumber<int>();
                child = child->nextSibling();
            }
        }
        else if (token->rule() == UriSyntax::instance()->path()) {
            path_ = decode(text->copy(token->i0(), token->i1()));
        }
        else if (token->rule() == UriSyntax::instance()->query()) {
            query_ = decode(text->copy(token->i0(), token->i1()));
        }
        else if (token->rule() == UriSyntax::instance()->fragment()) {
            fragment_ = decode(text->copy(token->i0(), token->i1()));
        }
        token = token->nextSibling();
    }
}

String Uri::requestHost() const
{
    return host_ + ":" + str(port_);
}

String Uri::requestPath() const
{
    if (query_ == "" && fragment_ == "") return path_;
    auto parts = StringList::create();
    parts << path_;
    if (query_ != "") parts << "?" << query_;
    if (fragment_ != "") parts << "#" << fragment_;
    return parts;
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

String Uri::encode(const String &s)
{
    mutate(s)->downcaseInsitu();

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
                mutate(pct)->at(1) = ch >> 4;
                mutate(pct)->at(2) = ch & 0xF;
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

String Uri::decode(const String &s)
{
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
        if (j < i) mutate(s)->at(j) = ch;
    }
    mutate(s)->truncate(j);
    return s;
}

String UriSyntaxError::message() const
{
    return Format("Invalid URI syntax: \"%%\"") << text_;
}

}} // namespace cc::net
