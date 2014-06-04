/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/Format.h>
#include <flux/File.h>
#include "HtmlWriter.h"

namespace fluxtoki
{

HtmlWriter::HtmlWriter(Theme *theme)
	: theme_(theme)
{}

void HtmlWriter::writeCss()
{
	Ref<File> file = File::open(theme_->name() + ".css", File::Create|File::Truncate|File::WriteOnly);
	Format format(file);
	for (int i = 0; i < theme_->paletteCount(); ++i) {
		const Palette *palette = theme_->paletteAt(i);
		String scopeName = palette->scopeName();
		for (int j = 0; j < palette->styleCount(); ++j) {
			const Style *style = palette->styleAt(j);
			format << "." << scopeName << "." << style->ruleName() << "{\n";
			if (style->ink() != Color()) format << "color: " << style->ink() << ";\n";
			if (style->paper() != Color()) format << "background-color: " << style->paper() << ";\n";
			if (style->bold()) format << "font-weight: bold;\n";
			if (style->italic()) format << "font-style: italic;\n";
			format << "}\n" << flush;
		}
	}
}

} // namespace fluxtoki
