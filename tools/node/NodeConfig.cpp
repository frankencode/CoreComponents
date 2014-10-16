/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/File>
#include <flux/Dir>
#include <flux/Singleton>
#include <flux/Arguments>
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

	MetaObject *nodePrototype = configProtocol()->lookup("Node");
	arguments->validate(nodePrototype);

	Ref<MetaObject> config;

	if (items->count() > 0) {
		if (items->count() > 1)
			throw UsageError("Handling multiple input arguments at once is not supported");

		String path = items->at(0);
		if (Dir::exists(path)) {
			directoryPath_ = path;
		}
		else {
			config = yason::parse(File::open(path)->map(), configProtocol());
		}
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
	for (int i = 0; i < address_->count(); ++i)
		address_->at(i)->setPort(port);

	user_ = config->value("user");
	version_ = config->value("version");
	daemon_ = config->value("daemon");
	serviceWindow_ = config->value("service_window");
	errorLogConfig_ = LogConfig::load(cast<MetaObject>(config->value("error_log")));
	accessLogConfig_ = LogConfig::load(cast<MetaObject>(config->value("access_log")));

	serviceInstances_ = ServiceInstances::create();
	if (config->hasChildren()) {
		for (int i = 0; i < config->children()->count(); ++i) {
			MetaObject *child = config->children()->at(i);
			ServiceDefinition *service = serviceRegistry()->serviceByName(child->className());
			serviceInstances_->append(service->createInstance(child));
		}
	}
}

NodeConfig *nodeConfig() { return Singleton<NodeConfig>::instance(); }

} // namespace fluxnode
