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

class BinaryTree
{
public:
    virtual void setRoot(BinaryNode *k) = 0;

    inline virtual void changed(BinaryNode */*kp*/, BinaryNode */*kc*/, bool /*left*/, bool /*attached*/) {}

    void attach(BinaryNode *kp, BinaryNode *kn, bool left);
    BinaryNode *detach(BinaryNode *k);
    void replace(BinaryNode *ki, BinaryNode *kl);
    BinaryNode *unlink(BinaryNode *k);

    void attachBefore(BinaryNode *kb, BinaryNode *kn);
    void attachAfter(BinaryNode *ka, BinaryNode *kn);

    static BinaryNode *min(BinaryNode *k);
    static BinaryNode *max(BinaryNode *k);
    static BinaryNode *pred(BinaryNode *k);
    static BinaryNode *succ(BinaryNode *k);

    inline static int weight(BinaryNode *k) { return (k) ? k->weight_ : 0; }
    inline static void establishWeight(BinaryNode *k) { k->weight_ = weight(k->left_) + weight(k->right_) + 1; }
};

} // namespace cc
