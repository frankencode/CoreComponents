/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/exceptions>

namespace cc {
namespace syntax {

class SyntaxState;

/** \brief Syntax error
  */
class SyntaxError: public TextError
{
public:
    SyntaxError(String text, SyntaxState *state = 0, String resource = "");
    ~SyntaxError() throw();

    inline const SyntaxState *state() const { return state_; }

    virtual String message() const;

private:
    Ref<SyntaxState> state_;
};

}} // namespace cc::syntax

