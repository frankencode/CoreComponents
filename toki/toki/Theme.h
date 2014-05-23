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

#include <flux/YasonObject.h>
#include "Palette.h"

namespace fluxtoki
{

using namespace flux;

class Theme: public YasonObject
{
public:
	inline String name() const { return name_; }

protected:
	Theme();

	virtual void define();
	virtual void realize(const ByteArray *text, Token *objectToken);

	inline void addPalette(Palette* palette) {
		paletteByScopeId_->insert(palette->scope(), palette);
	}

private:
	String name_;

	typedef Map<int, Ref<Palette> > PaletteByScopeId;
	Ref<PaletteByScopeId> paletteByScopeId_;
};

} // namespace fluxtoki

#endif // FLUXTOKI_THEME_H
