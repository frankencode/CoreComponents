 /*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/TextField>
#include <cc/ui/TextInput>

namespace cc {
namespace ui {

TextField::Instance::Instance(const String &labelText):
    InputField::Instance{labelText}
{}

InputControl TextField::Instance::createInputControl()
{
    return TextInput{};
}

}} // namespace cc::ui
