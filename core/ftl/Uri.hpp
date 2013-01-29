 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_URI_HPP
#define FTL_URI_HPP

#include "Token.hpp"
#include "string.hpp"

namespace ftl
{

FTL_EXCEPTION(UriException, Exception);

class Uri: public Instance
{
public:
	inline static hook<Uri> create() {
		return new Uri;
	}
	inline static hook<Uri> create(const char *text) {
		return new Uri(text);
	}
	inline static hook<Uri> create(ByteArray *bytes, Token *rootToken = 0) {
		return new Uri(bytes, rootToken);
	}

	inline string scheme() const { return scheme_; }
	inline void setScheme(string value) { scheme_ = value; }

	inline string userInfo() const { return userInfo_; }
	inline void setUserInfo(string value) { userInfo_ = value; }

	inline string host() const { return host_; }
	inline void setHost(string value) { host_ = value; }

	inline int port() const { return port_; }
	inline void setPort(int value) { port_ = value; }

	inline string path() const { return path_; }
	inline void setPath(string value) { path_ = value; }

	inline string query() const { return query_; }
	inline void setQuery(string value) { query_ = value; }

	inline string fragment() const { return fragment_; }
	inline void setFragment(string value) { fragment_ = value; }

	string toString() const;

private:
	Uri();
	Uri(const char *text);
	Uri(ByteArray *bytes, Token *rootToken = 0);

	void readUri(ByteArray *bytes, Token *rootToken = 0);
	static string encode(string s);
	static string decode(string s);

	string scheme_;
	string userInfo_;
	string host_;
	int port_;
	string path_;
	string query_;
	string fragment_;
};

} // namespace ftl

#endif // FTL_URI_HPP
