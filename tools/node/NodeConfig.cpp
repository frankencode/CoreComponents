/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/Arguments.h>
#include <flux/File.h>
#include "NodeConfigProtocol.h"
#include "ServiceRegistry.h"
#include "ErrorLog.h"
#include "NodeConfig.h"

namespace fluxnode
{

NodeConfig::NodeConfig()
{}

void NodeConfig::load(int argc, char **argv)
{
	Ref<Arguments> arguments = Arguments::parse(argc, argv);
	Ref<StringList> items = arguments->items();

	YasonObject *nodePrototype = configProtocol()->value("Node");
	arguments->validate(nodePrototype);

	Ref<YasonObject> config;

	if (items->size() > 0) {
		if (items->size() > 1)
			throw UsageError("Loading multiple config files at once is not supported");

		String path = items->at(0);
		config = yason::parse(File::open(path)->map(), configProtocol());
	}

	if (!config) config = nodePrototype->clone();
	arguments->override(config);

	String address = config->value("address");
	int port = config->value("port");
	String protocol = config->value("protocol");

	int family = AF_UNSPEC;
	if (protocol->downcase() == "ipv6") family = AF_INET6;
	else if (protocol->downcase() == "ipv4") family = AF_INET;

	address_ = SocketAddress::resolve(address, "http", family, SOCK_STREAM);
	for (int i = 0; i < address_->size(); ++i)
		address_->at(i)->setPort(port);

	user_ = config->value("user");
	version_ = config->value("version");
	daemon_ = config->value("daemon");
	serviceWindow_ = config->value("service_window");
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

} // namespace fluxnode
