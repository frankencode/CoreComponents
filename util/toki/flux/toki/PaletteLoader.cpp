/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/Singleton.h>
#include <flux/File.h>
#include <flux/ResourceGuard.h>
#include "PaletteLoader.h"

namespace flux {
namespace toki {

PaletteLoader::PaletteLoader():
	protocol_(MetaProtocol::create())
{
	protocol_->define<Palette>();
	protocol_->minCount(1);
	protocol_->maxCount(1);
}

Ref<Palette> PaletteLoader::load(String path) const
{
	ResourceGuard context(path);
	String text = File::open(path)->map();
	return yason::parse(text, protocol_);
}

const PaletteLoader *paletteLoader() { return Singleton<PaletteLoader>::instance(); }

}} // namespace flux::toki
