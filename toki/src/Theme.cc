/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Bundle>
#include <cc/Dir>
#include <cc/Format>
#include <cc/exceptions>
#include <cc/toki/Theme>

namespace cc {
namespace toki {

Ref<Theme> Theme::load(const String &path)
{
    String themePath = path;
    if (themePath->isRelativePath()) {
        if (!Dir::exists(themePath)) {
            if (!themePath->contains('/'))
                themePath = String{"themes/"} + themePath;
            if (!Dir::exists(themePath))
                themePath = CC_BUNDLE_LOOKUP(themePath);
        }
    }
    if (themePath == "" || !Dir::exists(themePath))
        throw UsageError{Format{"Failed to locate theme \"%%\""} << path};
    return new Theme{themePath};
}

Theme::Theme(const String &path):
    path_{path},
    name_{path->fileName()},
    paletteByScope_{PaletteByScope::create()}
{
    Ref<Dir> dir = Dir::open(path);
    String name;
    while (dir->read(&name)) {
        if (name == "." || name == "..") continue;
        if (!(name->count() > 0 && ('a' <= name->at(0) && name->at(0) <= 'z'))) continue;
        Ref<Palette> palette = Palette::load(path + "/" + name);
        paletteByScope_->insert(palette->scope(), palette);
    }
    if (!paletteByScope_->lookup(Palette::defaultScope(), &defaultPalette_))
        throw UsageError{Format{"Palette \"default\" missing in theme \"%%\""} << path};
}

Ref<StringList> themeList(const String &path)
{
    auto list = StringList::create();
    String searchPath = path;
    if (searchPath == "") {
        searchPath = CC_BUNDLE_LOOKUP("themes");
        list->appendList(themeList("themes"));
    }
    if (searchPath != "" && Dir::exists(searchPath)) {
        auto dir = Dir::open(searchPath);
        for (String name; dir->read(&name);) {
            if (name == "." || name == "..") continue;
            list->append(path + "/" + name);
        }
    }
    return list;
}

}} // namespace cc::toki
