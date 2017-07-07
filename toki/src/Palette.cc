/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ResourceContext>
#include <cc/Format>
#include <cc/toki/PaletteLoader>
#include <cc/toki/Registry>
#include <cc/toki/Palette>

namespace cc {
namespace toki {

Ref<Palette> Palette::load(String path)
{
    Ref<Palette> palette = paletteLoader()->load(path);
    return palette;
}

Palette::Palette():
    styleByRule_(StyleByRule::create())
{}

int Palette::defaultScope()
{
    return SyntaxDefinition::scope("default");
}

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

Ref<MetaObject> Palette::produce()
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
    scopeName_ = ResourceContext::instance()->top()->fileName();
    if (scopeName_ == "default") {
        scope_ = SyntaxDefinition::scope(scopeName_);
        for (int i = 0; i < children()->count(); ++i) {
            Style *style = Object::cast<Style *>(children()->at(i));
            style->rule_ = defaultRuleByName(style->ruleName());
            if (style->rule_ == Undefined) {
                Token *token = childToken(objectToken, i);
                token = valueToken(text, token, "name");
                throw SemanticError(
                    Format("Undefined default style '%%'") << style->ruleName(),
                    text, token->i1()
                );
            }
            styleByRule_->establish(style->rule_, style);
        }
        return;
    }

    Language *language = 0;
    if (!registry()->lookupLanguageByName(scopeName_, &language))
        throw SemanticError(Format("Undefined language '%%'") << scopeName_);

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
            throw SemanticError(ex.message(), text, token->i1());
        }
    }
}

}} // namespace cc::toki
