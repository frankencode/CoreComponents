/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>
#include <cc/PrefixTree>
#include <cc/Map>

namespace cc { template<class> class Singleton; }

namespace ccnode {

using namespace cc;

class MediaTypeDatabase: public Object
{
public:
    String lookup(String path, String content) const;

private:
    friend class Singleton<MediaTypeDatabase>;
    MediaTypeDatabase();

    Ref< PrefixTree<char, String> > mediaTypeByPathSuffix_;
    Ref< PrefixTree<char, String> > mediaTypeByContentPrefix_;
};

const MediaTypeDatabase *mediaTypeDatabase();

} // namespace ccnode

