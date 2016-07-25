/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <arpa/inet.h>
#include <cc/exceptions>
#include <cc/syntax/Token>
#include <cc/String>

namespace cc {
namespace net {

using syntax::Token;

/** \brief Uniform Resource Indicator
  */
class Uri: public Object
{
public:
    inline static Ref<Uri> create() {
        return new Uri;
    }
    inline static Ref<Uri> parse(String text, Token *rootToken = 0) {
        return new Uri(text, rootToken);
    }

    inline String scheme() const { return scheme_; }
    inline void setScheme(String value) { scheme_ = value; }

    inline String userInfo() const { return userInfo_; }
    inline void setUserInfo(String value) { userInfo_ = value; }

    inline String host() const { return host_; }
    inline bool hostIsNumeric() const { return hostIsNumeric_; }
    inline int family() const {
        if (!hostIsNumeric_) return AF_UNSPEC;
        return host_->contains(':') ? AF_INET6 : AF_INET;
    }
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
    Uri();
    Uri(ByteArray *text, Token *rootToken = 0);

    void readUri(ByteArray *text, Token *rootToken = 0);
    static String encode(String s);
    static String decode(String s);

    String scheme_;
    String userInfo_;
    String host_;
    bool hostIsNumeric_;
    int port_;
    String path_;
    String query_;
    String fragment_;
};

class UriSyntaxError: public UsageError
{
public:
    UriSyntaxError(String text): text_(text) {}
    String text() const { return text_; }
    virtual String message() const;

private:
    String text_;
};

}} // namespace cc::net
