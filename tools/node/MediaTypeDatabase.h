/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_MEDIATYPEDATABASE_H
#define FLUXNODE_MEDIATYPEDATABASE_H

#include <flux/String>
#include <flux/PrefixTree>
#include <flux/Map>

namespace flux { template<class> class Singleton; }

namespace fluxnode
{

using namespace flux;

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

} // namespace fluxnode

#endif // FLUXNODE_MEDIATYPEDATABASE_H
