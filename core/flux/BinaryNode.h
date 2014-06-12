/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_BINARYNODE_H
#define FLUX_BINARYNODE_H

namespace flux
{

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
