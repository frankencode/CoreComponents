/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>
#include <cc/Token>
#include <cc/syntax/SyntaxDefinition>

namespace ccclaim {

using namespace cc;
using namespace cc::syntax;

class Header: public Object
{
public:
    static Ref<Header> create(const String &path, Token *token, const String &text, const String &message) {
        return new Header{path, token, text, message};
    }

    String path() const { return path_; }
    Token *token() const { return token_; } // FIXME: const Token *
    String text() const { return text_; }
    String message() const { return message_; }

private:
    Header(const String &path, Token *token, const String &text, const String &message):
        path_{path},
        token_{token},
        text_{text},
        message_{message}
    {}

    String path_;
    Ref<Token> token_;
    String text_;
    String message_;
};

} // namespace ccclaim
