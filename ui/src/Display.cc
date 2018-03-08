/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Display>

namespace cc {
namespace ui {

Size Display::getSizeInCm() const
{
    return (size_ / dpi_) * 2.54;
}

double Display::getDiagonalInInch() const
{
    return (size_ / dpi_)->magnitude();
}


}} // namespace cc::ui
