/*
 * Uri.cpp -- Uniform Resource Identifier according to RFC3986
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "UriSyntax.hpp"
#include "Uri.hpp"

namespace ftl
{

Uri::Uri(Ref<ByteArray> bytes, Ref<Uri> baseUri, Ref<Token> rootToken)
{
	Ref<Token, Owner> token = rootToken;
	if (!token) {
		token = uriSyntax()->match(bytes);
		if (!token) FTL_THROW(UriException, "URI decomposition failed, invalid syntax");
	}
	
	token = token->firstChild();
	if (token->rule() == uriSyntax()->scheme()) {
		scheme_ = bytes->copy(token->i0(), token->i1());
		token = token->nextSibling();
	}
	
	FTL_CHECK(token->rule() == uriSyntax()->ruleByName("hier-part")->id());
	
}

String Uri::toString() const
{
	StringList l;
	if (scheme_ != "") {
		l.append(scheme_);
		l.append(":");
	}
	if ((userInfo_ != "") || (host_ != "")) {
		l.append("//");
		if (userInfo_ != "")
			l.append(userInfo_);
		if (host_ != "") {
			l.append("@");
			l.append(host_);
		}
	}
	l.append(path_);
	if (query_ != "") {
		l.append("?");
		l.append(query_);
	}
	if (fragment_ != "") {
		l.append("#");
		l.append(fragment_);
	}
	return l.join();
}

String Uri::percentEncode(String s)
{
	const char* reserved = ":/?#[]@!$&'()*+,;=";
	StringList l;
	int j = 0;
	for (int i = 0, n = s->length(); i < n; ++i) {
		char ch = s->at(i);
		for (const char* r = reserved; *r; ++r) {
			if (ch == *r) {
				if (j < i)
					l.append(s->copy(j, i));
				String pct("%XX");
				pct->set(1, ch >> 4);
				pct->set(2, ch & 0xF);
				l.append(pct);
				j = i + 1;
				break;
			}
		}
	}
	if (j == 0) return s;
	if (j < s->length())
		l.append(s->copy(j, s->length()));
	return l.join();
}

} // namespace ftl
