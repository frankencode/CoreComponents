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

/** \class Uri Uri.h cc/net/Uri
  * \brief Uniform Resource Indicator
  */
class Uri: public Object
{
public:
    /** Create a new URI
      * \return new object instance
      */
    inline static Ref<Uri> create() {
        return new Uri;
    }

    /** Parse an URI
      * \param text URI string
      * \param rootToken root of the URI token tree (optional)
      * \return new object intance
      * \exception UriSyntaxError
      */
    inline static Ref<Uri> parse(String text, Token *rootToken = 0) {
        return new Uri(text, rootToken);
    }

    /// URI scheme (e.g. http, file)
    inline String scheme() const { return scheme_; }

    /// %Set the URI scheme
    inline void setScheme(String value) { scheme_ = value; }

    /// User and password information (e.g. "oscar:123456")
    inline String userInfo() const { return userInfo_; }

    /// %Set the user information
    inline void setUserInfo(String value) { userInfo_ = value; }

    /// Registered host name or numeric address (IPv4 or IPv6)
    inline String host() const { return host_; }

    /// %Set the host name
    inline void setHost(String value) { host_ = value; }

    /// Tell if the host name is a numeric address
    inline bool hostIsNumeric() const { return hostIsNumeric_; }

    /// Address family of the numeric address (AF_INET or AF_INET6)
    inline int family() const {
        if (!hostIsNumeric_) return AF_UNSPEC;
        return host_->contains(':') ? AF_INET6 : AF_INET;
    }

    /// Service port
    inline int port() const { return port_; }

    /// %Set the service port
    inline void setPort(int value) { port_ = value; }

    /// Path component of the URI
    inline String path() const { return path_; }

    /// %Set the path component of the URI
    inline void setPath(String value) { path_ = value; }

    /// Query component of the URI
    inline String query() const { return query_; }

    /// %Set the query component of the URI
    inline void setQuery(String value) { query_ = value; }

    /// Fragment component of the URI
    inline String fragment() const { return fragment_; }

    /// %Set fragment component of the URI
    inline void setFragment(String value) { fragment_ = value; }

    /// Convert this URI to a string
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

/** \class UriSyntaxError Uri.h cc/net/Uri
  * \brief Failed to parse an URI string
  */
class UriSyntaxError: public UsageError
{
public:
    /** Low-level constructor
      * \param text \copydoc text()
      */
    UriSyntaxError(String text): text_(text) {}

    /// the text that failed to parse as an URI
    String text() const { return text_; }

    virtual String message() const override;

private:
    String text_;
};

}} // namespace cc::net
