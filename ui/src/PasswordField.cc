/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/PasswordInput>
#include <cc/ui/PasswordField>

namespace cc {
namespace ui {

PasswordField::PasswordField(View *parent, const String &labelText, const String &bullet):
    InputField(parent, labelText),
    bullet_(bullet)
{}

String PasswordField::password() const
{
    return passwordInput_->password();
}

InputControl *PasswordField::addInputControl()
{
    return passwordInput_ = add<PasswordInput>(bullet_);
}

}} // namespace cc::ui
