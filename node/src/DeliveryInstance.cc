/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/DeliveryInstance>
#include <cc/node/MediaTypeDatabase>
#include <cc/node/LoggingRegistry>
#include <cc/stdio>

namespace cc {
namespace node {

DeliveryInstance::DeliveryInstance(const MetaObject *config):
    serviceName_{config->className()},
    requestLimit_{config->value("request-limit")},
    requestPayloadLimit_{config->value("request-payload-limit")},
    host_{String{config->value("host")}},
    uri_{String{config->value("uri")}},
    security_{HttpServerSecurity::load(Variant::cast<const MetaObject *>(config->value("security")))},
    mediaTypeDatabase_{MediaTypeDatabase::instance()},
    loggingInstances_{LoggingInstances::create()}
{
    for (const MetaObject *child: config->children()) {
        const LoggingService *service = LoggingRegistry::instance()->serviceByName(child->className());
        loggingInstances_->append(service->createInstance(child));
    }
    if (loggingInstances_->count() == 0) {
        if (stdErr()->isatty()) {
            const LoggingService *service = LoggingRegistry::instance()->serviceByName("Foreground-Log");
            loggingInstances_->append(service->createInstance(service->configPrototype()));
        }
    }
}

DeliveryInstance::~DeliveryInstance()
{}

void DeliveryInstance::logDelivery(const HttpServerConnection *client, int statusCode, size_t bytesWritten, const String &statusMessage) const
{
    for (const LoggingInstance *loggingInstance: loggingInstances_)
        loggingInstance->logDelivery(client, statusCode, bytesWritten, statusMessage);
}

void DeliveryInstance::logStatus(const String &message, LoggingLevel level) const
{
    for (const LoggingInstance *loggingInstance: loggingInstances_)
        loggingInstance->logStatus(message, level);
}

}} // namespace cc::node
