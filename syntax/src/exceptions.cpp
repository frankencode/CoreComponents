/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Format>
#include <flux/syntax/SyntaxState>
#include <flux/syntax/exceptions>

namespace flux {

SyntaxError::SyntaxError(String text, SyntaxState *state, String resource)
    : TextError(text, state ? state->hintOffset() : -1, resource),
      state_(state)
{}

SyntaxError::~SyntaxError() throw()
{}

String SyntaxError::message() const
{
    Format format;
    const char *error = "Syntax error";
    if (state_) if (state_->hint()) {
        int line = 0, pos = 0;
        text_->offsetToLinePos(state_->hintOffset(), &line, &pos);
        if (resource_ != "") format << resource_ << ":";
        if (!text_->contains('\n') && text_->trim()->count() > 0) format << "\'" << text_ << "\':";
        format << line << ":" << pos << ": ";
    }
    format << error;
    if (state_) if (state_->hint()) format << ": " << state_->hint();
    return format;
}

} // namespace flux
