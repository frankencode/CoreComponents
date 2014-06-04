/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXTOKI_THEME_H
#define FLUXTOKI_THEME_H

#include "Palette.h"

namespace fluxtoki
{

using namespace flux;

class Theme: public Object
{
public:
	static Ref<Theme> load(String path);
	inline String path() const { return path_; }
	inline String name() const { return name_; }

	inline bool lookupPaletteByScope(int scope, Palette **palette) const {
		return paletteByScope_->lookup(scope, palette);
	}

	inline int paletteCount() const { return paletteByScope_->size(); }
	inline const Palette *paletteAt(int i) const { return paletteByScope_->valueAt(i); }

private:
	Theme(String path);

	String path_;
	String name_;

	typedef Map<int, Ref<Palette> > PaletteByScope;
	Ref<PaletteByScope> paletteByScope_;
};

} // namespace fluxtoki

#endif // FLUXTOKI_THEME_H
