#include <flux/YasonObject.h>
#include "LogConfig.h"
#include "ServiceInstance.h"

namespace fluxnode
{

ServiceInstance::ServiceInstance(YasonObject *config)
	: serviceName_(config->className()),
	  concurrency_(config->value("concurrency")),
	  connectionTimeout_(config->value("connection_timeout")),
	  host_(String(config->value("host"))),
	  uri_(String(config->value("uri"))),
	  errorLogConfig_(LogConfig::load(cast<YasonObject>(config->value("error_log")))),
	  accessLogConfig_(LogConfig::load(cast<YasonObject>(config->value("access_log")))),
	  visitLogConfig_(LogConfig::load(cast<YasonObject>(config->value("visit_log"))))
{}

} // namespace fluxnode
