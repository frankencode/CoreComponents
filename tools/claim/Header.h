/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>
#include <cc/syntax/Token>

namespace ccclaim {

using namespace cc;
using namespace cc::syntax;

class Header: public Object
{
public:
    inline static Ref<Header> create(String path, Token *token, String text, String message) {
        return new Header(path, token, text, message);
    }

    inline String path() const { return path_; }
    inline Token *token() const { return token_; }
    inline String text() const { return text_; }
    inline String message() const { return message_; }

private:
    Header(String path, Token *token, String text, String message)
        : path_(path),
          token_(token),
          text_(text),
          message_(message)
    {}

    String path_;
    Ref<Token> token_;
    String text_;
    String message_;
};

} // namespace ccclaim

