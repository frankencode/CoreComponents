/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/PasswordEditor>
#include <cc/ui/PasswordInput>

namespace cc {
namespace ui {

PasswordInput::PasswordInput(View *parent, const String &bullet):
    TextInput{parent},
    passwordEditor_{Object::create<PasswordEditor>(bullet)}
{
    font->bind([=]{ return app()->defaultFixedFont(); });
}

String PasswordInput::password() const
{
    return passwordEditor_->password();
}

Ref<TextEditor> PasswordInput::createEditor()
{
    return passwordEditor_;
}

}} // namespace cc::ui
