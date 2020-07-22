/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/PasswordInput>
#include <cc/ui/PasswordEditor>
#include <cc/ui/Application>

namespace cc {
namespace ui {

PasswordInput::Instance::Instance(const String &bullet):
    passwordEditor_{Object::create<PasswordEditor>(bullet)}
{
    font <<[=]{ return Application{}->defaultFixedFont(); };
}

String PasswordInput::Instance::password() const
{
    return passwordEditor_->password();
}

Ref<TextEditor> PasswordInput::Instance::createEditor(const String &)
{
    return passwordEditor_;
}

}} // namespace cc::ui
