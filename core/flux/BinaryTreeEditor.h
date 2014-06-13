/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_BINARYTREEEDITOR_H
#define FLUX_BINARYTREEEDITOR_H

#include "BinaryNode.h"

namespace flux
{

class BinaryTreeEditor
{
public:
	virtual void setRoot(BinaryNode *k) = 0;

	inline virtual void changed(BinaryNode *kp, BinaryNode *kc/*<= FIXME: redundant*/, bool left, bool attached) {}

	void attach(BinaryNode *kp, BinaryNode *kn, bool left);
	BinaryNode *detach(BinaryNode *k);
	void replaceNode(BinaryNode *ki, BinaryNode *kl);
};

} // namespace flux

#endif // FLUX_BINARYTREEEDITOR_H
