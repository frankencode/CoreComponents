/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Token>

namespace cc {

bool Token::project(TokenScreen *screen)
{
    int i = i0_;

    Token *child = firstChild();
    while (child) {
        if (i < child->i0_) {
            if (!screen->project(this, i, child->i0_))
                return false;
            i = child->i0_;
        }
        if (!child->project(screen))
            return false;
        i = child->i1_;
        child = child->nextSibling();
    }

    if (i < i1_)
        if (!screen->project(this, i, i1_))
            return false;

    return true;
}

Token *Token::at(int i) const
{
    Token *token = const_cast<Token *>(this);
    Token *child = firstChild();
    while (child) {
        if ((child->i0() <= i) && (i < child->i1())) {
            token = child->at(i);
            break;
        }
        child = child->nextSibling();
    }
    return token;
}

} // namespace cc
