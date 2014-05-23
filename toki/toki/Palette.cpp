/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "Registry.h"
#include "Palette.h"

namespace fluxtoki
{

Palette::Palette()
	: styleByRule_(StyleByRule::create())
{}

void Palette::define()
{
	className("Palette");
	insert("language", "");
	protocol()->minCount(1);
	protocol()->define<Style>();
}

void Palette::realize(const ByteArray *text, Token *objectToken)
{
	language_ = value("language");
	if (language_ == "default") {
		// default palette, TODO
		/*
		default::Text,
		default::CurrentLine,
		default::Cursor,
		default::Selection,
		default::Match,
		default::LineNumber,
		default::CurrentLineNumber
		*/
		return;
	}
	Language *language = 0;
	if (!registry()->lookupLanguageByName(language_, &language)) {
		// TODO
		return;
	}
	SyntaxDefinition *scope = language->syntax();
	scope_ = scope->id();
	for (int i = 0; i < children()->size(); ++i) {
		Style *style = cast<Style>(children()->at(i));
		int rule;
		/* TODO
		if (!scope->lookupRuleIdByName(style->name(), &ruleId)) {
			// TODO
		}
		*/
		styleByRule_->insert(rule, style);
	}
}

} // namespace fluxtoki
