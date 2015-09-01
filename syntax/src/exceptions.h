/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXSYNTAX_EXCEPTIONS_H
#define FLUXSYNTAX_EXCEPTIONS_H

#include <flux/exceptions>

namespace flux {

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

} // namespace flux

#endif // FLUXSYNTAX_EXCEPTIONS_H
