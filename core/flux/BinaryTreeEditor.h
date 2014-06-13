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
	void replaceNode(BinaryNode *ki, BinaryNode *kl); // FIXME: rename to "replace"
	BinaryNode *removeNode(BinaryNode *k); // FIXME: rename to unlink

	void attachBefore(BinaryNode *kb, BinaryNode *kn);
	void attachAfter(BinaryNode *ka, BinaryNode *kn);

	static BinaryNode *min(BinaryNode *k);
	static BinaryNode *max(BinaryNode *k);
	static BinaryNode *pred(BinaryNode *k);
	static BinaryNode *succ(BinaryNode *k);

	inline static int weight(BinaryNode *k) { return (k) ? k->weight_ : 0; }
	inline static void establishWeight(BinaryNode *k) { k->weight_ = weight(k->left_) + weight(k->right_) + 1; }
};

} // namespace flux

#endif // FLUX_BINARYTREEEDITOR_H
