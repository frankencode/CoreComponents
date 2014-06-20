/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/ResourceContext.h>
#include <flux/Format.h>
#include "PaletteLoader.h"
#include "Registry.h"
#include "Palette.h"

namespace flux {
namespace toki {

Ref<Palette> Palette::load(String path)
{
	Ref<Palette> palette = paletteLoader()->load(path);
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
	if (name == "Selection") return Selection;
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
	scopeName_ = resourceContextStack()->top()->fileName();
	if (scopeName_ == "default") {
		for (int i = 0; i < children()->count(); ++i) {
			Style *style = cast<Style>(children()->at(i));
			int rule = defaultRuleByName(style->ruleName());
			if (rule == Undefined) {
				Token *token = childToken(objectToken, i);
				token = valueToken(text, token, "name");
				throw SemanticError(
					Format("Undefined default style '%%'") << style->ruleName(),
					text, token->i1()
				);
			}
			defaultStyleByRule_->establish(rule, style);
		}
		return;
	}

	Language *language = 0;
	if (!registry()->lookupLanguageByName(scopeName_, &language))
		throw SemanticError(Format("Undefined language '%%'") << scopeName_);

	const SyntaxDefinition *syntax = language->highlightingSyntax();
	scope_ = syntax->id();
	for (int i = 0; i < children()->count(); ++i) {
		Style *style = cast<Style>(children()->at(i));
		try {
			style->rule_ = syntax->ruleByName(style->ruleName());
			styleByRule_->insert(style->rule_, style);
		}
		catch (DebugError &ex) {
			Token *token = childToken(objectToken, i);
			token = valueToken(text, token, "name");
			throw SemanticError(ex.message(), text, token->i1());
		}
	}
}

}} // namespace flux::toki
