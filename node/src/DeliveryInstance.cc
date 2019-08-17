/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/DeliveryInstance>
#include <cc/node/MediaTypeDatabase>
#include <cc/node/LogConfig>

namespace cc {
namespace node {

DeliveryInstance::DeliveryInstance(const MetaObject *config):
    serviceName_{config->className()},
    requestLimit_{config->value("request-limit")},
    requestPayloadLimit_{config->value("request-payload-limit")},
    host_{String{config->value("host")}},
    uri_{String{config->value("uri")}},
    security_{HttpServerSecurity::load(Variant::cast<MetaObject *>(config->value("security")))},
    mediaTypeDatabase_{MediaTypeDatabase::instance()}
{}

DeliveryInstance::~DeliveryInstance()
{}

}} // namespace cc::node
