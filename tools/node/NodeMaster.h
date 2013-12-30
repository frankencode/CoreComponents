/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXNODE_NODEMASTER_H
#define FLUXNODE_NODEMASTER_H

#include <flux/Object.h>
#include <flux/Singleton.h>

namespace fluxnode
{

using namespace flux;

class NodeMaster: public Object, public Singleton<NodeMaster>
{
public:
	int run(int argc, char **argv);

private:
	friend class Singleton<NodeMaster>;

	NodeMaster();
	void runNode(int argc, char **argv);
	void runNode();
};

inline NodeMaster* nodeMaster() { return NodeMaster::instance(); }

} // namespace fluxnode

#endif // FLUXNODE_NODEMASTER_H
