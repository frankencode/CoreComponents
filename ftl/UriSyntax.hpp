/*
 * UriSyntax.hpp -- URI syntax according to RFC3986
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef FTL_URISYNTAX_HPP
#define FTL_URISYNTAX_HPP

#include "AbnfCoreSyntax.hpp"
#include "Singleton.hpp"

namespace ftl
{

class UriSyntax: public AbnfCoreSyntax, public Singleton<UriSyntax>
{
public:
	inline int uri() const { return uri_; }
	inline int hierPart() const { return hierPart_; }
	inline int uriReference() const { return uriReference_; }
	inline int absoluteUri() const { return absoluteUri_; }
	inline int relativeRef() const { return relativeRef_; }
	inline int scheme() const { return scheme_; }
	inline int authority() const { return authority_; }
	inline int userInfo() const { return userInfo_; }
	inline int host() const { return host_; }
	inline int port() const { return port_; }
	inline int inet6Address() const { return inet6Address_; }
	inline int h16() const { return h16_; }
	inline int ls32() const { return ls32_; }
	inline int inetAddress() const { return inetAddress_; }
	inline int decOctet() const { return decOctet_; }
	inline int regName() const { return regName_; }
	inline int path() const { return path_; }
	inline int pathAbEmpty() const { return pathAbEmpty_; }
	inline int pathAbsolute() const { return pathAbsolute_; }
	inline int pathNoScheme() const { return pathNoScheme_; }
	inline int pathRootless() const { return pathRootless_; }
	inline int pathEmpty() const { return pathEmpty_; }
	inline int segment() const { return segment_; }
	inline int segmentNz() const { return segmentNz_; }
	inline int segmentNzNc() const { return segmentNzNc_; }
	inline int query() const { return query_; }
	inline int fragment() const { return fragment_; }
	inline int pctEncoded() const { return pctEncoded_; }
	
private:
	UriSyntax(Ref<DebugFactory> debugFactory = 0);
	friend class Singleton<UriSyntax>;
	
	int uri_;
	int hierPart_;
	int uriReference_;
	int absoluteUri_;
	int relativeRef_;
	int relativePart_;
	int scheme_;
	int authority_;
	int userInfo_;
	int host_;
	int port_;
	int inet6Address_;
	int h16_;
	int ls32_;
	int inetAddress_;
	int decOctet_;
	int regName_;
	int path_;
	int pathAbEmpty_;
	int pathAbsolute_;
	int pathNoScheme_;
	int pathRootless_;
	int pathEmpty_;
	int segment_;
	int segmentNz_;
	int segmentNzNc_;
	int query_;
	int fragment_;
	int pctEncoded_;
};

inline Ref<UriSyntax> uriSyntax() { return UriSyntax::instance(); }

} // namespace ftl

#endif // FTL_URISYNTAX_HPP
