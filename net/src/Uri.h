/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNET_URI_H
#define FLUXNET_URI_H

#include <flux/syntax/exceptions>
#include <flux/syntax/Token>
#include <flux/String>

namespace flux {
namespace net {

/** \brief Uniform Resource Indicator
  */
class Uri: public Object
{
public:
    inline static Ref<Uri> create() {
        return new Uri;
    }
    inline static Ref<Uri> create(const char *text) {
        return new Uri(text);
    }
    inline static Ref<Uri> create(ByteArray *text, Token *rootToken = 0) {
        return new Uri(text, rootToken);
    }

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
    Uri();
    Uri(const char *text);
    Uri(ByteArray *text, Token *rootToken = 0);

    void readUri(ByteArray *text, Token *rootToken = 0);
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

}} // namespace flux::net

#endif // FLUXNET_URI_H
