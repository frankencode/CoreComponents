/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/Singleton.h>
#include "LogPrototype.h"
#include "NodeConfigProtocol.h"

namespace fluxnode
{

class NodePrototype: public YasonObject
{
public:
	static Ref<NodePrototype> create(YasonProtocol *protocol = 0, String className = "Node")
	{
		return new NodePrototype(className, protocol);
	}

protected:
	NodePrototype(String className, YasonProtocol *protocol)
		: YasonObject(className, protocol)
	{
		insert("address", "localhost");
		insert("port", 8080);
		insert("protocol", "");
		insert("user", "");
		insert("version", "Fnode/0.1.2");
		insert("daemon", false);
		insert("service_window", 30);
		insert("error_log", LogPrototype::create());
		insert("access_log", LogPrototype::create());
	}
};

NodeConfigProtocol::NodeConfigProtocol()
	: nodeProtocol_(YasonProtocol::create())
{
	Ref<NodePrototype> nodePrototype = NodePrototype::create(nodeProtocol_);
	define(nodePrototype);
}

void NodeConfigProtocol::registerService(YasonObject *configPrototype)
{
	nodeProtocol_->define(configPrototype);
}

NodeConfigProtocol *configProtocol() { return Singleton<NodeConfigProtocol>::instance(); }

} // namespace fluxnode
