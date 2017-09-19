/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Registry>
#include <cc/ui/ApplicationProvider>

namespace cc {
namespace ui {

ApplicationProvider::ApplicationProvider()
{
    Registry::instance()->registerApplicationProvider(this);
}

}} // namespace cc::ui
