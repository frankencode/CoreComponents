/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/PasswordEdit>
#include <cc/PasswordInput>
#include <cc/PasswordInputState>

namespace cc {

PasswordEdit::PasswordEdit(const String &label, Out<PasswordEdit> self):
    LineEdit{PasswordInput{}, label}
{
    self = weak<PasswordEdit>();
}

String PasswordEdit::password() const
{
    return input().as<PasswordInput>().password();
}

} // namespace cc
