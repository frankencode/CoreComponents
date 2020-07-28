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

PasswordField::Instance::Instance(const String &label, const String &bullet):
    InputField::Instance{PasswordInput{bullet}, label}
{}

String PasswordField::Instance::password() const
{
    return input_->as<PasswordInput>()->password();
}

}} // namespace cc::ui
