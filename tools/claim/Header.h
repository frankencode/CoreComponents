/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXCLAIM_HEADER_H
#define FLUXCLAIM_HEADER_H

#include <flux/String>
#include <flux/syntax/Token>

namespace fluxclaim {

using namespace flux;

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

} // namespace fluxclaim

#endif // FLUXCLAIM_HEADER_H
