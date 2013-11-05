/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FNODE_MEDIATYPEDATABASE_H
#define FNODE_MEDIATYPEDATABASE_H

#include <flux/Singleton.h>
#include <flux/String.h>
#include <flux/PrefixTree.h>
#include <flux/Map.h>

namespace fnode
{

using namespace flux;

class MediaTypeDatabase: public Object, public Singleton<MediaTypeDatabase>
{
public:
	String lookup(String path, String content) const;

private:
	friend class Singleton<MediaTypeDatabase>;
	MediaTypeDatabase();

	Ref< PrefixTree<char, String> > mediaTypeByPathSuffix_;
	Ref< PrefixTree<char, String> > mediaTypeByContentPrefix_;
};

inline MediaTypeDatabase *mediaTypeDatabase() { return MediaTypeDatabase::instance(); }

} // namespace fnode

#endif // FNODE_MEDIATYPEDATABASE_H
