/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/string>
#include <cc/Token>
#include <cc/syntax/SyntaxDefinition>

namespace ccclaim {

using namespace cc;
using namespace cc::syntax;

class Header: public Object
{
public:
    static Ref<Header> create(const string &path, Token *token, const string &text, const string &message) {
        return new Header{path, token, text, message};
    }

    string path() const { return path_; }
    Token *token() const { return token_; } // FIXME: const Token *
    string text() const { return text_; }
    string message() const { return message_; }

private:
    Header(const string &path, Token *token, const string &text, const string &message):
        path_{path},
        token_{token},
        text_{text},
        message_{message}
    {}

    string path_;
    Ref<Token> token_;
    string text_;
    string message_;
};

} // namespace ccclaim
