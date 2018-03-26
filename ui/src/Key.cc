/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Format>
#include <cc/ui/Key>

namespace cc {
namespace ui {

String str(Key key)
{
    return Format()
        << "Key {" << nl
        << "  scanCode: " << key->scanCode() << nl
        << "  keyCode: " << key->keyCode() << nl
        << "  modifiers: " << key->modifiers() << nl
        << "}";
}

}} // namspace cc::ui
