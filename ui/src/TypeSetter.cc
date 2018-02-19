/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/PlatformPlugin>
#include <cc/ui/TypeSetter>

namespace cc {
namespace ui {

Ref<TypeSetter> TypeSetter::create()
{
    return PlatformPlugin::instance()->createTypeSetter();
}

}} // namespace cc::ui
