/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/toki/Palette>

namespace cc {
namespace toki {

class Theme: public Object
{
public:
    static Ref<Theme> load(String path);
    inline String path() const { return path_; }
    inline String name() const { return name_; }

    inline bool lookupPaletteByScope(int scope, Palette **palette) const {
        return paletteByScope_->lookup(scope, palette);
    }

    inline int paletteCount() const { return paletteByScope_->count(); }
    inline const Palette *paletteAt(int i) const { return paletteByScope_->valueAt(i); }

    inline const Palette *defaultPalette() const { return defaultPalette_; }

private:
    Theme(String path);

    String path_;
    String name_;

    typedef Map<int, Ref<Palette> > PaletteByScope;
    Ref<PaletteByScope> paletteByScope_;
    Ref<Palette> defaultPalette_;
};

Ref<StringList> themeList(String path = "");

}} // namespace cc::toki

