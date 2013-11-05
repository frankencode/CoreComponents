/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/Config.h>
#include "ConfigProtocol.h"
#include "ServiceRegistry.h"
#include "ErrorLog.h"
#include "NodeConfig.h"

namespace fnode
{

NodeConfig::NodeConfig()
{}

void NodeConfig::load(int argc, char **argv)
{
	YasonObject *nodePrototype = configProtocol()->value("Node");
	Ref<Config> config = Config::read(argc, argv, nodePrototype);
	if (config->arguments()->size() > 0) {
		if (config->arguments()->size() > 1)
			throw UserException("Loading multiple config files at once is not supported");
		String path = config->arguments()->at(0);
		config = Config::read(path, configProtocol());
		config = Config::read(argc, argv, config);
	}

	String address = config->value("address", "localhost");
	int port = config->value("port", 8080);
	String protocol = config->value("protocol");

	int family = AF_UNSPEC;
	if (protocol->downcase() == "ipv6") family = AF_INET6;
	else if (protocol->downcase() == "ipv4") family = AF_INET;

	address_ = SocketAddress::resolve(address, "", family, SOCK_STREAM, &canonicalName_);
	for (int i = 0; i < address_->size(); ++i)
		address_->at(i)->setPort(port);

	user_ = config->value("user");
	version_ = config->value("version");
	daemon_ = config->value("daemon");
	errorLogConfig_ = LogConfig::load(cast<YasonObject>(config->value("error_log")));
	accessLogConfig_ = LogConfig::load(cast<YasonObject>(config->value("access_log")));

	serviceInstances_ = ServiceInstances::create();
	if (config->hasChildren()) {
		for (int i = 0; i < config->children()->size(); ++i) {
			YasonObject *child = config->children()->at(i);
			ServiceDefinition *service = serviceRegistry()->serviceByName(child->className());
			serviceInstances_->append(service->createInstance(child));
		}
	}
}

} // namespace fnode
