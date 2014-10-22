/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/MetaObject>
#include "LogConfig.h"
#include "ServiceInstance.h"

namespace fluxnode {

ServiceInstance::ServiceInstance(MetaObject *config):
    serviceName_(config->className()),
    concurrency_(config->value("concurrency")),
    connectionTimeout_(config->value("connection_timeout")),
    host_(String(config->value("host"))),
    uri_(String(config->value("uri"))),
    errorLogConfig_(LogConfig::load(cast<MetaObject>(config->value("error_log")))),
    accessLogConfig_(LogConfig::load(cast<MetaObject>(config->value("access_log")))),
    visitLogConfig_(LogConfig::load(cast<MetaObject>(config->value("visit_log"))))
{}

} // namespace fluxnode
