/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Format>
#include <cc/syntax/SyntaxState>
#include <cc/syntax/exceptions>

namespace cc {
namespace syntax {

SyntaxError::SyntaxError(const String &text, SyntaxState *state, const String &resource):
    TextError{text, state ? state->hintOffset() : -1, resource},
    state_{state}
{}

String SyntaxError::message() const
{
    Format f;
    const char *error = "Syntax error";
    if (state_) if (state_->hint()) {
        int line = 0, pos = 0;
        text_->offsetToLinePos(state_->hintOffset(), &line, &pos);
        if (resource_ != "") f << resource_ << ":";
        if (!text_->contains('\n') && text_->trim()->count() > 0) f << "\'" << text_ << "\':";
        f << line << ":" << pos << ": ";
    }
    f << error;
    if (state_) if (state_->hint()) f << ": " << state_->hint();
    return f;
}

}} // namespace cc
