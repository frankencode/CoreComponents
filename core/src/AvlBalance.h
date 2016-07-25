/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/BinaryNode>

namespace cc {

class AvlBalance
{
protected:
    virtual void setRoot(BinaryNode *k) = 0;

    inline virtual void rotated(BinaryNode *k1, bool left) {}

    void restore(BinaryNode *kp, BinaryNode*, bool left, bool attached);

private:
    BinaryNode *rebalance(BinaryNode *k1);
    void rotate(BinaryNode *k1, bool left);

public:
#ifndef NDEBUG
    static int height(BinaryNode *k);
    static bool testBalance(BinaryNode *k);
#endif
};

} // namespace cc
