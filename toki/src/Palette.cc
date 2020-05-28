/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/toki/Palette>
#include <cc/toki/PaletteLoader>
#include <cc/toki/Registry>
#include <cc/ResourceContext>
#include <cc/Format>

namespace cc {
namespace toki {

Ref<Palette> Palette::load(const String &path)
{
    return PaletteLoader::instance()->load(path);
}

Palette::Palette():
    styleByRule_{StyleByRule::create()}
{}

int Palette::defaultScope()
{
    return SyntaxDefinition::scope("default");
}

int Palette::defaultRuleByName(const String &name)
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

void Palette::realize()
{
    scopeName_ = ResourceContext::instance()->top()->fileName();
    if (scopeName_ == "default") {
        scope_ = SyntaxDefinition::scope(scopeName_);
        for (MetaObject *child: children()) {
            Style *style = static_cast<Style *>(child);
            style->rule_ = defaultRuleByName(style->ruleName());
            if (style->rule_ == Undefined) {
                throw MetaError{
                    Format{"Undefined rule '%%'"} << style->ruleName(),
                    style, "rule"
                };
            }
            styleByRule_->establish(style->rule_, style);
        }
        return;
    }

    Language *language = nullptr;
    if (!Registry::instance()->lookupLanguageByName(scopeName_, &language))
        throw SemanticError{Format{"Undefined language '%%'"} << scopeName_};

    const SyntaxDefinition *syntax = language->highlightingSyntax();
    scope_ = syntax->id();
    for (MetaObject *child: children()) {
        Style *style = static_cast<Style *>(child);
        try {
            style->rule_ = syntax->ruleByName(style->ruleName());
        }
        catch (DebugError &ex) {
            throw MetaError{
                Format{"Undefined rule '%%'"} << style->ruleName(),
                style, "rule"
            };
        }
        styleByRule_->insert(style->rule_, style);
    }
}

}} // namespace cc::toki
