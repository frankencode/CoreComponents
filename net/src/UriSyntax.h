/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNET_URISYNTAX_H
#define FLUXNET_URISYNTAX_H

#include <flux/syntax/SyntaxDefinition>

namespace flux {

template<class SubClass> class Singleton;

namespace net {

/** \brief Syntax of an URI
  */
class UriSyntax: public SyntaxDefinition
{
public:
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

    UriSyntax(SyntaxDebugFactory *debugFactory = 0);

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

UriSyntax *uriSyntax();

}} // namespace flux::net

#endif // FLUXNET_URISYNTAX_H
