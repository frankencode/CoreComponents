/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXTOKI_THEME_H
#define FLUXTOKI_THEME_H

#include <flux/toki/Palette>

namespace flux {
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

}} // namespace flux::toki

#endif // FLUXTOKI_THEME_H
