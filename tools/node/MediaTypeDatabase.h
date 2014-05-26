/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXNODE_MEDIATYPEDATABASE_H
#define FLUXNODE_MEDIATYPEDATABASE_H

#include <flux/String.h>
#include <flux/PrefixTree.h>
#include <flux/Map.h>

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
