/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <stdio.h> // DEBUG
#include <flux/Bundle.h>
#include <flux/Dir.h>
#include <flux/Format.h>
#include <flux/exceptions.h>
#include "Theme.h"

namespace fluxtoki
{

Ref<Theme> Theme::load(String path)
{
	String themePath = path;
	if (themePath->isRelativePath()) {
		if (!Dir::exists(themePath)) {
			if (!themePath->contains('/'))
				themePath = String("themes/") + themePath;
			if (!Dir::exists(themePath))
				themePath = FLUX_BUNDLE_LOOKUP(themePath);
		}
	}
	if (themePath == "" || !Dir::exists(themePath))
		throw UsageError(Format("Failed to locate theme directory: %%") << path);
	return new Theme(themePath);
}

Theme::Theme(String path)
	: path_(path),
	  name_(path->fileName()),
	  paletteByScope_(PaletteByScope::create())
{
	Ref<Dir> dir = Dir::open(path);
	String name;
	while (dir->read(&name)) {
		if (name == "." || name == "..") continue;
		if (!(name->size() > 0 && ('a' <= name->at(0) && name->at(0) <= 'z'))) continue;
		Ref<Palette> palette = Palette::load(path + "/" + name);
		paletteByScope_->insert(palette->scope(), palette);
	}
}

} // namespace fluxtoki
