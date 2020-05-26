/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/DeliveryInstance>
#include <cc/http/MediaTypeDatabase>
#include <cc/stdio>

namespace cc {
namespace http {

DeliveryInstance::DeliveryInstance(const MetaObject *config):
    serviceName_{config->className()},
    requestLimit_{config->value("request-limit")},
    requestPayloadLimit_{config->value("request-payload-limit")},
    host_{string{config->value("host")}},
    uri_{string{config->value("uri")}},
    security_{HttpServerSecurity::load(Variant::cast<const MetaObject *>(config->value("security")))},
    mediaTypeDatabase_{MediaTypeDatabase::instance()}
    #if 0
    TODO...
    errorLoggingInstance_{},
    accessLoggingInstance_{}
    #endif
{}

DeliveryInstance::~DeliveryInstance()
{}

}} // namespace cc::http
