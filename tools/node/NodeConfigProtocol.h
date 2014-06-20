/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXNODE_NODECONFIGPROTOCOL_H
#define FLUXNODE_NODECONFIGPROTOCOL_H

#include <flux/MetaProtocol.h>

namespace flux { template<class> class Singleton; }

namespace fluxnode
{

using namespace flux;

class NodeConfigProtocol: public MetaProtocol
{
public:
	void registerService(MetaObject *configPrototype);

private:
	friend class Singleton<NodeConfigProtocol>;
	NodeConfigProtocol();
	Ref<MetaProtocol> nodeProtocol_;
};

NodeConfigProtocol *configProtocol();

} // namespace fluxnode

#endif // FLUXNODE_NODECONFIGPROTOCOL_H
