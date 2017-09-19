/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/ui/Image>
#include <cc/ui/GlyphStop>
#include <cc/ui/FontMetrics>
#include <cc/ui/TextBlock>

namespace cc {
namespace ui {

class TypeSetter: public Object
{
public:
    static Ref<TypeSetter> create();

    inline bool caching() const { return caching_; }
    inline void setCaching(bool on) { caching_ = on; }

    void layout(TextBlock *block, double *penX, double *penY, Image *image = 0) const;
    void pace(TextBlock *block, double *width = 0, double *height = 0) const;

protected:
    TypeSetter(): caching_(false) {}

    GlyphRun *getRun(TextBlock *block) { return block->run_; }
    virtual void stage(TextBlock *block, double *penX, double *penY, Image *image = 0, GlyphRun *run = 0) const = 0;

    static double extent(GlyphRun *run);
    static double extent(GlyphStop *first, GlyphStop *last);

    void elide(TextBlock *block, GlyphRun *run) const;
    void wrap(TextBlock *block, GlyphRun *run) const;
    void spread(TextBlock *block, GlyphRun *run, int i0, int i1) const;

    bool caching_;
};

}} // namespace cc::ui

