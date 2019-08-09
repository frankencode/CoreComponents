/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/meta/MetaObject>
#include "MediaTypeDatabase.h"
#include "LogConfig.h"
#include "ServiceInstance.h"

namespace ccnode {

ServiceInstance::ServiceInstance(MetaObject *config):
    serviceName_{config->className()},
    requestLimit_{config->value("request-limit")},
    requestPayloadLimit_{config->value("request-payload-limit")},
    host_{String{config->value("host")}},
    uri_{String{config->value("uri")}},
    security_{HttpServerSecurity::load(Variant::cast<MetaObject *>(config->value("security")))},
    mediaTypeDatabase_{MediaTypeDatabase::instance()}
{}

ServiceInstance::~ServiceInstance()
{}

} // namespace ccnode
