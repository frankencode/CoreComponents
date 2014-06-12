/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_AVLBALANCE_H
#define FLUX_AVLBALANCE_H

#include "BinaryNode.h"

namespace flux
{

class AvlBalance
{
protected:
	virtual void rotate(BinaryNode *k1, bool left) = 0;
	void restore(BinaryNode *kp, BinaryNode*, bool left, bool attached);
	BinaryNode *rebalance(BinaryNode *k1);
#ifndef NDEBUG
	static int height(BinaryNode *k);
	static bool testBalance(BinaryNode *k);
#endif
};

} // namespace flux

#endif // FLUX_AVLBALANCE_H
