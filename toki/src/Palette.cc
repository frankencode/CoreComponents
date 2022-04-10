/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/toki/Palette>
#include <cc/toki/PalettePrototype>
#include <cc/toki/Registry>
#include <cc/ResourceContext>
#include <cc/ResourceGuard>
#include <cc/TextError>
#include <cc/MetaError>
#include <cc/Crc32Sink>
#include <cc/File>
#include <cc/Format>
#include <cc/yason>

namespace cc::toki {

struct Palette::State final: public MetaObject::State
{
    void realize() override
    {
        String languageName = ResourceContext{}.top().fileName();

        if (languageName == "default") {
            for (const MetaObject &child: children_) {
                Style style = child.as<Style>();
                if (style.ruleName() == "Text") {
                    textStyle_ = style;
                    break;
                }
            }
            if (!textStyle_) {
                throw MetaError{
                    "Style \"Text\" is missing in \"default\" palette"
                };
            }

            return;
        }

        Language language;
        if (!Registry{}.lookupLanguageByName(languageName, &language)) {
            throw TextError(Format{"Undefined language '%%'"} << languageName);
        }

        HighlightingSyntax syntax = language.highlightingSyntax();

        syntax.clearMetaData();

        for (const MetaObject &child: children_) {
            Style style = child.as<Style>();
            SyntaxRule rule;
            if (!syntax.ruleByName().lookup(style.ruleName(), &rule)) {
                throw MetaError{
                    Format{"Undefined rule '%%'"} << style.ruleName(),
                    style, "rule"
                };
            }
            rule.setMetaData(style);
        }
    }

    Style textStyle_;
};

Palette::Palette(New):
    MetaObject{new State}
{}

Palette::Palette(const String &path)
{
    static MetaProtocol protocol = []{
        MetaProtocol protocol;
        protocol.define(PalettePrototype{});
        protocol.setMinCount(1);
        protocol.setMaxCount(1);
        return protocol;
    }();

    ResourceGuard context{path};
    String text = File{path}.map();
    *this = yasonParse(text, protocol).to<MetaObject>().as<Palette>();

}

Style Palette::textStyle() const
{
    return me().textStyle_;
}

long Palette::count() const
{
    return me().children_.count();
}

Style Palette::at(long i) const
{
    return me().children_.at(i).as<Style>();
}

const Palette::State &Palette::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::toki
