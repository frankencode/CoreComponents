/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/syntax/SyntaxDefinition>

namespace cc { template<class> class Singleton; }

namespace cc {
namespace net {

/** \brief Syntax of an URI
  */
class UriSyntax: public syntax::SyntaxDefinition
{
public:
    static const UriSyntax *instance();

    inline int userInfo() const { return userInfo_; }
    inline int host() const { return host_; }
    inline int port() const { return port_; }

    inline int scheme() const { return scheme_; }
    inline int authority() const { return authority_; }
    inline int path() const { return path_; }
    inline int query() const { return query_; }
    inline int fragment() const { return fragment_; }

    inline int uri() const { return uri_; }

protected:
    friend class Singleton<UriSyntax>;

    UriSyntax(syntax::SyntaxDebugFactory *debugFactory = 0);

    int userInfo_;
    int host_;
    int port_;
    int scheme_;
    int authority_;
    int path_;
    int query_;
    int fragment_;
    int uri_;
};

}} // namespace cc::net
