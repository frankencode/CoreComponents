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

PasswordEdit::PasswordEdit(const String &label):
    LineEdit{PasswordInput{}, label}
{}

PasswordEdit &PasswordEdit::associate(Out<PasswordEdit> self)
{
    return View::associate<PasswordEdit>(self);
}

String PasswordEdit::password() const
{
    return input().as<PasswordInput>().password();
}

} // namespace cc