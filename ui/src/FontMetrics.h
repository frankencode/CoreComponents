/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Object>

namespace cc {
namespace ui {

class Font;

class FontMetrics: public Object
{
public:
    static Ref<FontMetrics> get(Font *font);

    virtual double fontSize() const = 0;
    virtual double ascender() const = 0;
    virtual double descender() const = 0;
    virtual double lineHeight() const = 0;
    virtual double capitalHeight() const = 0;
};

}} // namespace cc::ui

