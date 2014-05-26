/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/Singleton.h>
#include "MediaTypeDatabase.h"

namespace fluxnode
{

MediaTypeDatabase::MediaTypeDatabase()
	: mediaTypeByPathSuffix_(PrefixTree<char, String>::create()),
	  mediaTypeByContentPrefix_(PrefixTree<char, String>::create())
{
	mediaTypeByPathSuffix_->insert("xhtml", "application/xhtml+xml");
	mediaTypeByPathSuffix_->insert("css", "text/css");
	mediaTypeByPathSuffix_->insert("js", "text/javascript");
	mediaTypeByPathSuffix_->insert("svg", "image/svg+xml");
	mediaTypeByContentPrefix_->insert("<!DOCTYPE html", "text/html");
	mediaTypeByContentPrefix_->insert("<html", "text/html");
	mediaTypeByContentPrefix_->insert("<?xml", "application/xml");
	mediaTypeByContentPrefix_->insert("<svg", "image/svg+xml");
	mediaTypeByContentPrefix_->insert("\377\330", "image/jpeg");
	mediaTypeByContentPrefix_->insert("\211PNG", "image/png");
	mediaTypeByContentPrefix_->insert("II*\0", "image/tiff");
	mediaTypeByContentPrefix_->insert("MM*\0", "image/tiff");
	mediaTypeByContentPrefix_->insert("GIF87a", "image/gif");
	mediaTypeByContentPrefix_->insert("GIF89a", "image/gif");
}

String MediaTypeDatabase::lookup(String path, String content) const
{
	String value;
	if (path != "") {
		String suffix = path->copy(path->find('.'), path->size());
		if (suffix != "") {
			if (mediaTypeByPathSuffix_->lookup(suffix, &value, false))
				return value;
		}
	}
	mediaTypeByContentPrefix_->lookup(content, &value, false);
	return value;
}

const MediaTypeDatabase *mediaTypeDatabase() { return Singleton<MediaTypeDatabase>::instance(); }

} // namespace fluxnode
