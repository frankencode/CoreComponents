/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_NODEMASTER_H
#define FLUXNODE_NODEMASTER_H

#include <flux/Thread.h>

namespace flux { template<class> class Singleton; }

namespace fluxnode
{

using namespace flux;

class NodeMaster: public Thread
{
public:
	static int run(int argc, char **argv);

private:
	friend class Singleton<NodeMaster>;

	NodeMaster();

	virtual void run();
	void runNode() const;

	int exitCode_;
};

// NodeMaster* nodeMaster();

} // namespace fluxnode

#endif // FLUXNODE_NODEMASTER_H
