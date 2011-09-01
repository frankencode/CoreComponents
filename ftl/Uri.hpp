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
	Uri();
	Uri(const char* text);
	Uri(Ref<ByteArray> bytes, Ref<Token> rootToken = 0);
	
	inline String scheme() const { return scheme_; }
	inline void setScheme(String value) { scheme_ = value; }
	
	inline String userInfo() const { return userInfo_; }
	inline void setUserInfo(String value) { userInfo_ = value; }
	
	inline String host() const { return host_; }
	inline void setHost(String value) { host_ = value; }
	
	inline int port() const { return port_; }
	inline void setPort(int value) { port_ = value; }
	
	inline String path() const { return path_; }
	inline void setPath(String value) { path_ = value; }
	
	inline String query() const { return query_; }
	inline void setQuery(String value) { query_ = value; }
	
	inline String fragment() const { return fragment_; }
	inline void setFragment(String value) { fragment_ = value; }
	
	String toString() const;
	
private:
	void readUri(Ref<ByteArray> bytes, Ref<Token> rootToken = 0);
	static String encode(String s);
	static String decode(String s);
	
	String scheme_;
	String userInfo_;
	String host_;
	int port_;
	String path_;
	String query_;
	String fragment_;
};

} // namespace ftl

#endif // FTL_URI_HPP
