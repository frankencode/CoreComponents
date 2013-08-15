/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FNODE_NODEMASTER_H
#define FNODE_NODEMASTER_H

#include <fkit/Object.h>
#include <fkit/Singleton.h>

namespace fnode
{

using namespace fkit;

class NodeMaster: public Object, public Singleton<NodeMaster>
{
public:
	int run(int argc, char **argv);

private:
	friend class Singleton<NodeMaster>;

	NodeMaster();
	void runNode(int argc, char **argv);
};

inline NodeMaster* nodeMaster() { return NodeMaster::instance(); }

} // namespace fnode

#endif // FNODE_NODEMASTER_H
