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

#include <flux/Singleton.h>
#include <flux/Config.h>

namespace fluxnode
{

using namespace flux;

class NodeConfigProtocol: public ConfigProtocol, public Singleton<NodeConfigProtocol>
{
public:
	void registerService(YasonObject *configPrototype);

private:
	friend class Singleton<NodeConfigProtocol>;
	NodeConfigProtocol();
	Ref<YasonProtocol> nodeProtocol_;
};

inline NodeConfigProtocol *configProtocol() { return NodeConfigProtocol::instance(); }

} // namespace fluxnode

#endif // FLUXNODE_NODECONFIGPROTOCOL_H
