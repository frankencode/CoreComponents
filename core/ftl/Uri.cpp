 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */

#include "debug.hpp"
#include "Format.hpp"
#include "UriSyntax.hpp"
#include "Uri.hpp"

namespace ftl
{

Uri::Uri()
	: port_(-1)
{}

Uri::Uri(const char *text)
	: port_(-1)
{
	readUri(string(text));
}

Uri::Uri(ByteArray *bytes, Token *rootToken)
	: port_(-1)
{
	readUri(bytes, rootToken);
}

void Uri::readUri(ByteArray *bytes, Token *rootToken)
{
	hook<Token> rootToken2;
	if (!rootToken) {
		check(uriSyntax());
		rootToken2 = uriSyntax()->match(bytes);
		if (!rootToken2) FTL_THROW(UriException, "URI decomposition failed, invalid syntax");
		rootToken = rootToken2;
	}

	Token *token = rootToken;
	token = token->firstChild();

	while (token) {
		if (token->rule() == uriSyntax()->scheme()) {
			scheme_ = decode(bytes->copy(token->i0(), token->i1()));
		}
		else if (token->rule() == uriSyntax()->authority()) {
			Token *child = token->firstChild();
			while (child) {
				if (child->rule() == uriSyntax()->userInfo())
					userInfo_ = decode(bytes->copy(child->i0(), child->i1()));
				else if (child->rule() == uriSyntax()->host())
					host_ = decode(bytes->copy(child->i0(), child->i1()));
				else if (child->rule() == uriSyntax()->port())
					port_ = decode(bytes->copy(child->i0(), child->i1()))->toInt();
				child = child->nextSibling();
			}
		}
		else if (token->rule() == uriSyntax()->path()) {
			path_ = decode(bytes->copy(token->i0(), token->i1()));
		}
		else if (token->rule() == uriSyntax()->query()) {
			query_ = decode(bytes->copy(token->i0(), token->i1()));
		}
		else if (token->rule() == uriSyntax()->fragment()) {
			fragment_ = decode(bytes->copy(token->i0(), token->i1()));
		}
		token = token->nextSibling();
	}
}

string Uri::toString() const
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

string Uri::encode(string s)
{
	s->toLowerInsitu();

	const char *reserved = ":/?#[]@!$&'()*+,;=";
	hook<StringList> l = StringList::create();
	int j = 0;
	for (int i = 0, n = s->length(); i < n; ++i) {
		char ch = s->at(i);
		for (const char *r = reserved; *r; ++r) {
			if (ch == *r) {
				if (j < i)
					l->append(s->copy(j, i));
				string pct("%XX");
				pct->set(1, ch >> 4);
				pct->set(2, ch & 0xF);
				l->append(pct);
				j = i + 1;
				break;
			}
		}
	}
	if (j == 0) return s;
	if (j < s->length())
		l->append(s->copy(j, s->length()));
	return l->join();
}

string Uri::decode(string s)
{
	s->toLowerInsitu();

	int j = 0;
	for (int i = 0, n = s->length(); i < n; ++i, ++j) {
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
		if (j < i) s->set(j, ch);
	}
	if (j < s->length())
		s->truncate(j);
	return s;
}

} // namespace ftl
