/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

namespace cc {

class BinaryNode
{
public:
    BinaryNode()
        : weight_(1),
          balance_(0)
    {}
    BinaryNode(const BinaryNode &kb)
        : weight_(kb.weight_),
          balance_(kb.balance_)
    {}

    BinaryNode *left_;
    BinaryNode *right_;
    BinaryNode *parent_;
    int weight_;
    int balance_;
};

} // namespace cc
