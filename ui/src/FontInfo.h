/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>
#include <cc/ui/FontFace>

namespace cc {
namespace ui {

class FontInfo: public Object
{
public:
    virtual int id() const = 0;
    virtual String family() const = 0;

    virtual int faceCount() const = 0;
    virtual FontFace *faceAt(int i) const = 0;

    virtual bool fixedWidth() const;
};

}} // namespace cc::ui

