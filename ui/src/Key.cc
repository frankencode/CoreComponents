/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/format>
#include <cc/ui/Key>

namespace cc {
namespace ui {

string str(Key key)
{
    return format{}
        << "Key {" << nl
        << "  scanCode: " << key->scanCode() << nl
        << "  keyCode: " << key->keyCode() << nl
        << "  modifiers: " << key->modifiers() << nl
        << "}";
}

}} // namspace cc::ui
