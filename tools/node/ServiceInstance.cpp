#include <fkit/Yason.h>
#include "LogConfig.h"
#include "ServiceInstance.h"

namespace fnode
{

ServiceInstance::ServiceInstance(YasonObject *config)
	: serviceName_(config->className()),
	  concurrency_(config->value("concurrency")),
	  connectionTimeout_(config->value("connection_timeout")),
	  host_(String(config->value("host"))),
	  uri_(String(config->value("uri"))),
	  errorLogConfig_(LogConfig::load(cast<YasonObject>(config->value("error_log")))),
	  accessLogConfig_(LogConfig::load(cast<YasonObject>(config->value("access_log"))))
{}

} // namespace fnode
