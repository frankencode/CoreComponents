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
#include "PaletteLoader.h"

namespace fluxtoki
{

PaletteLoader::PaletteLoader():
	protocol_(YasonProtocol::create())
{
	protocol_->define<Palette>();
}

Ref<Palette> PaletteLoader::load(String path) const
{
	String text = File::open(path)->map();
	return yason::parse(text, protocol_);
}

const PaletteLoader *paletteLoader() { return Singleton<PaletteLoader>::instance(); }

} // namespace fluxtoki
