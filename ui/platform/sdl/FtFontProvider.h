/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Map>
#include <cc/ui/FontProvider>
#include <cc/ui/sdl/FtFontInfo>

namespace cc {
namespace ui {

class FtActivation;

class FtFontProvider: public FontProvider {
public:
    static FtFontProvider *create();
    void addPath(String path);

    int fontCount() const;
    FontInfo *fontInfo(int i) const;
    Ref<Font> selectFont(int i) const;
    Ref<Font> selectFont(String family) const;

    Ref<FontMetrics> getMetrics(Font *font) const;

private:
    friend class FtActivation;

    FtFontProvider();

    FtFontFace *selectFace(Font *font) const;
    FtFontFace *activateFace(Font *font) const;

    typedef Map<String, Ref<FtFontInfo> > InfoByFamily;
    typedef Map<int, Ref<FtFontInfo> > InfoById;
    Ref<InfoByFamily> infoByFamily_;
    Ref<InfoById> infoById_;

    float xRes_, yRes_;
};

}} // namespace cc::ui

