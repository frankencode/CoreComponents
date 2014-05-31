/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/yason.h>
#include <flux/exceptions.h>
#include "PaletteLoader.h"
#include "Registry.h"
#include "Palette.h"

namespace fluxtoki
{

Ref<Palette> Palette::load(String path)
{
	Ref<Palette> palette = paletteLoader()->load(path);
	palette->name_ = path->fileName();
	return palette;
}

Palette::Palette():
	defaultStyleByRule_(StyleByRule::create()),
	styleByRule_(StyleByRule::create())
{}

int Palette::defaultRuleByName(String name)
{
	if (name == "Text") return Text;
	if (name == "CurrentLine") return CurrentLine;
	if (name == "Cursor") return Cursor;
	if (name == "Match") return Match;
	if (name == "LineNumber") return LineNumber;
	if (name == "CurrentLineNumber") return CurrentLineNumber;
	return Undefined;
}

Ref<YasonObject> Palette::produce()
{
	return new Palette;
}

void Palette::define()
{
	className("Palette");
	protocol()->minCount(1);
	protocol()->define<Style>();
}

void Palette::realize(const ByteArray *text, Token *objectToken)
{
	if (name_ == "default") {
		for (int i = 0; i < children()->size(); ++i) {
			Style *style = cast<Style>(children()->at(i));
			int rule = defaultRuleByName(style->name());
			if (rule == Undefined)
				FLUX_DEBUG_ERROR(Format("Undefined default style '%%'") << style->name());
			defaultStyleByRule_->establish(rule, style);
		}
		return;
	}

	Language *language = 0;
	if (!registry()->lookupLanguageByName(name_, &language)) {
		FLUX_DEBUG_ERROR(Format("Undefined language '%%'") << name_);
	}
	const SyntaxDefinition *syntax = language->highlightingSyntax();
	scope_ = syntax->id();
	for (int i = 0; i < children()->size(); ++i) {
		Style *style = cast<Style>(children()->at(i));
		int rule = syntax->ruleByName(style->name());
		styleByRule_->insert(rule, style);
	}
}

} // namespace fluxtoki
