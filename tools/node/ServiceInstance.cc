/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/meta/MetaObject>
#include "LogConfig.h"
#include "SecurityConfig.h"
#include "ServiceInstance.h"

namespace ccnode {

ServiceInstance::ServiceInstance(MetaObject *config):
    serviceName_{config->className()},
    requestLimit_{config->value("request-limit")},
    requestPayloadLimit_{config->value("request-payload-limit")},
    host_{String{config->value("host")}},
    uri_{String{config->value("uri")}},
    security_{SecurityConfig::load(Variant::cast<MetaObject *>(config->value("security")))}
{}

ServiceInstance::~ServiceInstance()
{}

} // namespace ccnode
