/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/toki/Theme>
#include <cc/Bundle>
#include <cc/Dir>
#include <cc/Format>
#include <cc/exceptions>

namespace cc::toki {

struct Theme::State final: public Object::State
{
    static String locate(const String &path)
    {
        String themePath = path;
        if (themePath.isRelativePath()) {
            if (!Dir::exists(themePath)) {
                if (!themePath.contains('/')) {
                    themePath = String{"themes/"} + themePath;
                }
                if (!Dir::exists(themePath)) {
                    themePath = CC_BUNDLE_LOOKUP(themePath);
                }
            }
        }
        if (themePath == "" || !Dir::exists(themePath)) {
            throw UsageError{Format{"Failed to locate theme \"%%\""} << path};
        }
        return themePath;
    }

    State(const String &path):
        path_{locate(path)},
        name_{path.baseName()}
    {

        Dir dir{path_};
        for (const String &name: dir) {
            if (name == "." || name == "..") continue;
            if (!(name.count() > 0 && ('a' <= name.at(0) && name.at(0) <= 'z'))) continue;
            Palette palette{path_ + "/" + name};
            if (name == "default") defaultPalette_ = palette;
        }
    }

    String path_;
    String name_;
    Palette defaultPalette_;
};

Theme::Theme(const String &path):
    Object{new State{path}}
{}

String Theme::path() const
{
    return me().path_;
}

String Theme::name() const
{
    return me().name_;
}

Palette Theme::defaultPalette() const
{
    return me().defaultPalette_;
}

const Theme::State &Theme::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::toki
