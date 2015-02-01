/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Memory>
#include <flux/Token>

namespace flux {

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

} // namespace flux
