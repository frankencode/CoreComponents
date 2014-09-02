/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_NODEMASTER_H
#define FLUXNODE_NODEMASTER_H

#include <flux/Object.h>

namespace flux { template<class> class Singleton; }

namespace fluxnode
{

using namespace flux;

class NodeMaster: public Object
{
public:
	int run(int argc, char **argv) const;

private:
	friend class Singleton<NodeMaster>;

	NodeMaster();
	void runNode(int argc, char **argv) const;
	void runNode() const;
};

const NodeMaster* nodeMaster();

} // namespace fluxnode

#endif // FLUXNODE_NODEMASTER_H
