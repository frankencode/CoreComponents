/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_BINARYNODE_H
#define FLUX_BINARYNODE_H

namespace flux {

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

} // namespace flux

#endif // FLUX_BINARYNODE_H
