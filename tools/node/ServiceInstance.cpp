#include <fkit/Yason.h>
#include "ServiceInstance.h"

namespace fnode
{

ServiceInstance::ServiceInstance(YasonObject *config)
	: serviceName_(config->className()),
	  concurrency_(config->value("concurrency")),
	  stackSize_(config->value("stack_size")),
	  maxConnections_(config->value("max_connections")),
	  maxConnectionsPerClient_(config->value("max_connections_per_client")),
	  connectionTimeout_(config->value("connection_timeout")),
	  host_(String(config->value("host"))),
	  uri_(String(config->value("uri")))
{}

} // namespace fnode
