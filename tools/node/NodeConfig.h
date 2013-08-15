/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FNODE_NODECONFIG_H
#define FNODE_NODECONFIG_H

#include <fkit/SocketAddress.h>
#include <fkit/String.h>
#include <fkit/Map.h>
#include "ServiceDefinition.h"

namespace fnode
{

using namespace fkit;

class NodeConfig: public Object, public Singleton<NodeConfig>
{
public:
	void load(int argc, char **argv);

	inline String canoncialName() const { return canonicalName_; }
	inline SocketAddressList *address() const { return address_; }
	inline String user() const { return user_; }
	inline String version() const { return version_; }
	inline int logLevel() const { return logLevel_; }

	inline ServiceInstances *serviceInstances() const { return serviceInstances_; }

private:
	friend class Singleton<NodeConfig>;

	NodeConfig();

	String canonicalName_;
	Ref<SocketAddressList> address_;
	String user_;
	String version_;
	int logLevel_;

	Ref<ServiceInstances> serviceInstances_;
};

inline NodeConfig *nodeConfig() { return NodeConfig::instance(); }

} // namespace fnode

#endif // FNODE_NODECONFIG_H
