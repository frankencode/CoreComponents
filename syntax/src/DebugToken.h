/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/syntax/Token>

namespace cc { class Stream; }

namespace cc {
namespace syntax {

class DebugTokenFactory;

class DebugToken: public Token
{
    friend class DebugTokenFactory;

public:
    inline const char *scopeName() const { return scopeName_; }
    inline const char *ruleName() const { return ruleName_; }

    void printTo(Stream *stream, ByteArray *text, int depth = 0, int defaultScope = -1);

protected:
    DebugToken(int scope, int rule, const char *scopeName, const char *ruleName):
        Token(scope, rule),
        scopeName_(scopeName),
        ruleName_(ruleName)
    {}

private:
    const char *scopeName_;
    const char *ruleName_;
};

}} // namespace cc::syntax

