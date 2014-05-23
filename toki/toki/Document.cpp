/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/File.h>
#include "Document.h"

namespace fluxtoki
{

Ref<Document> Document::load(String path)
{
	return Document::create(File::open(path)->map(), path);
}

void Document::save()
{
	File::save(path_, text_);
}

} // namespace fluxtoki
