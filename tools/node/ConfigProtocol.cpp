/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "LogPrototype.h"
#include "ConfigProtocol.h"

namespace fnode
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
		insert("port", 8000);
		insert("protocol", "");
		insert("user", "");
		insert("version", "Fnode/0.0.1");
		insert("daemon", false);
		insert("error_log", LogPrototype::create());
		insert("access_log", LogPrototype::create());
	}
};

ConfigProtocol::ConfigProtocol()
	: nodeProtocol_(YasonProtocol::create())
{
	Ref<NodePrototype> nodePrototype = NodePrototype::create(nodeProtocol_);
	insert(nodePrototype->className(), nodePrototype);
}

void ConfigProtocol::registerService(YasonObject *configPrototype)
{
	nodeProtocol_->insert(configPrototype->className(), configPrototype);
}

} // namespace fnode
