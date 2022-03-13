/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Carrier>

namespace cc::ui {

Size Carrier::State::preferredSize() const
{
    return hasParent() ? parent().size() : View::State::preferredSize();
}

} // namespace cc::ui
