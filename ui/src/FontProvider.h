/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/ui/Font>
#include <cc/ui/FontInfo>
#include <cc/ui/FontMetrics>

namespace cc {
namespace ui {

class FontProvider: public Object
{
public:
    virtual void addPath(String path) = 0;

    virtual int fontCount() const = 0;
    virtual FontInfo *fontInfo(int i) const = 0;

    virtual Ref<Font> selectFont(int i) const = 0;
    virtual Ref<Font> selectFont(String family) const;

    virtual Ref<FontMetrics> getMetrics(Font *font) const = 0;

protected:
    static int faceSelector(bool italic, Font::Stretch stretch, Font::Weight weight);
    static Ref<Font> createFont(int id, String family);
};

}} // namespace cc::ui

