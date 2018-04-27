/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Format>
#include <cc/ui/Font>
#include <cc/ui/ScaledFont>

namespace cc {
namespace ui {

String ScaledFont::toString() const
{
    return "Scaled" + str(font());
}

}} // namespace cc::ui
