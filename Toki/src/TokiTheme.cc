/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TokiTheme>
#include <cc/Dir>
#include <cc/Format>
#include <cc/bundling>
#include <cc/exceptions>

namespace cc {

struct TokiTheme::State final: public Object::State
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
            TokiPalette palette{path_ + "/" + name};
            if (name == "default") defaultPalette_ = palette;
        }
    }

    String path_;
    String name_;
    TokiPalette defaultPalette_;
};

String TokiTheme::locate(const String &name)
{
    return TokiTheme::State::locate(name);
}

TokiTheme::TokiTheme(const String &path):
    Object{new State{path}}
{}

String TokiTheme::path() const
{
    return me().path_;
}

String TokiTheme::name() const
{
    return me().name_;
}

TokiPalette TokiTheme::defaultPalette() const
{
    return me().defaultPalette_;
}

const TokiTheme::State &TokiTheme::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
