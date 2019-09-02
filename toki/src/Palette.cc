/*
 * Copyright (C) 2007-2017 Frank Mertens.
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

Ref<MetaObject> Palette::produce() const
{
    return new Palette;
}

void Palette::realize(const CharArray *text, Token *objectToken)
{
    scopeName_ = ResourceContext::instance()->top()->fileName();
    if (scopeName_ == "default") {
        scope_ = SyntaxDefinition::scope(scopeName_);
        for (int i = 0; i < children()->count(); ++i) {
            Style *style = Object::cast<Style *>(children()->at(i));
            style->rule_ = defaultRuleByName(style->ruleName());
            if (style->rule_ == Undefined) {
                Token *token = childToken(objectToken, i);
                token = valueToken(text, token, "name");
                throw SemanticError{
                    Format{"Undefined default style '%%'"} << style->ruleName(),
                    text, token->i1()
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
    for (int i = 0; i < children()->count(); ++i) {
        Style *style = Object::cast<Style *>(children()->at(i));
        try {
            style->rule_ = syntax->ruleByName(style->ruleName());
            styleByRule_->insert(style->rule_, style);
        }
        catch (DebugError &ex) {
            Token *token = childToken(objectToken, i);
            token = valueToken(text, token, "name");
            throw SemanticError{ex->message(), text, token->i1()};
        }
    }
}

}} // namespace cc::toki
