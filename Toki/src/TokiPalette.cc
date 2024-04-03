/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TokiPalette>
#include <cc/TokiPalettePrototype>
#include <cc/TokiRegistry>
#include <cc/ResourceContext>
#include <cc/ResourceGuard>
#include <cc/TextError>
#include <cc/MetaError>
#include <cc/Crc32Sink>
#include <cc/File>
#include <cc/Format>
#include <cc/yason>

namespace cc {

struct TokiPalette::State final: public MetaObject::State
{
    void realize() override
    {
        String languageName = ResourceContext{}.top().fileName();

        if (languageName == "default") {
            for (const MetaObject &child: children_) {
                TokiStyle style = child.as<TokiStyle>();
                if (style.ruleName() == "Text") {
                    textStyle_ = style;
                    break;
                }
            }
            if (!textStyle_) {
                throw MetaError{
                    "TokiStyle \"Text\" is missing in \"default\" palette"
                };
            }

            return;
        }

        TokiLanguage language;
        if (!TokiRegistry{}.lookupLanguageByName(languageName, &language)) {
            throw TextError(Format{"Undefined language '%%'"} << languageName);
        }

        TokiHighlightingSyntax syntax = language.highlightingSyntax();

        syntax.clearMetaData();

        for (const MetaObject &child: children_) {
            TokiStyle style = child.as<TokiStyle>();
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

    TokiStyle textStyle_;
};

TokiPalette::TokiPalette(New):
    MetaObject{new State}
{}

TokiPalette::TokiPalette(const String &path)
{
    static MetaProtocol protocol = []{
        MetaProtocol protocol;
        protocol.define(TokiPalettePrototype{});
        protocol.setMinCount(1);
        protocol.setMaxCount(1);
        return protocol;
    }();

    ResourceGuard context{path};
    String text = File{path}.map();
    *this = yasonParse(text, protocol).to<MetaObject>().as<TokiPalette>();

}

TokiStyle TokiPalette::textStyle() const
{
    return me().textStyle_;
}

long TokiPalette::count() const
{
    return me().children_.count();
}

TokiStyle TokiPalette::at(long i) const
{
    return me().children_.at(i).as<TokiStyle>();
}

const TokiPalette::State &TokiPalette::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
