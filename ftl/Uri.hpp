/*
 * Uri.hpp -- Uniform Resource Identifier according to RFC3986
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef FTL_URI_HPP
#define FTL_URI_HPP

#include "Token.hpp"
#include "String.hpp"

namespace ftl
{

FTL_EXCEPTION(UriException, Exception);

class Uri: public Instance
{
public:
	Uri() {}
	Uri(Ref<ByteArray> bytes, Ref<Uri> baseUri = 0, Ref<Token> rootToken = 0);
	
	inline String scheme() const { return scheme_; }
	inline void setScheme(String value) { scheme_ = percentEncode(value); }
	
	inline String userInfo() const { return userInfo_; }
	inline void setUserInfo(String value) { userInfo_ = percentEncode(value); }
	
	inline String host() const { return host_; }
	inline void setHost(String value) { host_ = percentEncode(value); }
	
	inline String path() const { return path_; }
	inline void setPath(String value) { path_ = percentEncode(value); }
	
	inline String query() const { return query_; }
	inline void setQuery(String value) { query_ = percentEncode(value); }
	
	inline String fragment() const { return fragment_; }
	inline void setFragment(String value) { fragment_ = percentEncode(value); }
	
	String toString() const;
	
private:
	static String percentEncode(String s);
	
	String scheme_;
	String userInfo_;
	String host_;
	String path_;
	String query_;
	String fragment_;
};

} // namespace ftl

#endif // FTL_URI_HPP
