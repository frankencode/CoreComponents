/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/PasswordField>
#include <cc/ui/PasswordInput>

namespace cc {
namespace ui {

PasswordField::Instance::Instance(const String &labelText, const String &bullet):
    InputField::Instance{labelText},
    bullet_{bullet}
{}

String PasswordField::Instance::password() const
{
    return getPassword_();
}

InputControl PasswordField::Instance::createInputControl()
{
    PasswordInput input{bullet_};
    getPassword_ = [input]{ return input->password(); };
    return input;
}

}} // namespace cc::ui
