/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXTOKI_HTMLWRITER_H
#define FLUXTOKI_HTMLWRITER_H

#include <flux/Token.h>
#include <flux/Stream.h>
#include "Theme.h"

namespace fluxtoki
{

using namespace flux;

class HtmlWriter: public Object
{
public:
	static Ref<HtmlWriter> create(Theme *theme) { return new HtmlWriter(theme); }
	void writeCss();

private:
	HtmlWriter(Theme *theme);
	Ref<Theme> theme_;
};

} // namespace fluxtoki

#endif // FLUXTOKI_HTMLWRITER_H
