 /*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/TextInput>
#include <cc/ui/TextField>

namespace cc {
namespace ui {

TextField::TextField(View *parent, const String &labelText):
    InputField(parent, labelText)
{}

InputControl *TextField::addInputControl()
{
    return add<TextInput>();
}

}} // namespace cc::ui
