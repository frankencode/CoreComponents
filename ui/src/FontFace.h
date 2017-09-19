/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/ui/Font>

namespace cc {
namespace ui {

class FontFace: public Object
{
public:
    virtual bool fixedWidth() const = 0;
    virtual bool italic() const = 0;
    virtual Font::Stretch stretch() const = 0;
    virtual Font::Weight weight() const = 0;
};

}} // namespace cc::ui

