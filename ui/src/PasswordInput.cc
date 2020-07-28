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
    TextInput::Instance{Object::create<PasswordEditor>(bullet)}
{
    font <<[=]{ return Application{}->defaultFixedFont(); };
}

String PasswordInput::Instance::password() const
{
    return dynamic_cast<const PasswordEditor *>(editor())->password();
}

}} // namespace cc::ui
