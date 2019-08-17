/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/List>
#include <cc/glob/Pattern>
#include <cc/http/HttpServerSecurity>
#include "LogConfig.h"

namespace cc { namespace meta { class MetaObject; } }

namespace ccnode {

using namespace cc;

class DeliveryDelegate;
class DeliveryWorker;
class MediaTypeDatabase;

using cc::glob::Pattern;
using cc::http::HttpServerSecurity;

class DeliveryInstance: public Object
{
public:
    virtual Ref<DeliveryDelegate> createDelegate(DeliveryWorker *worker) const = 0;

    String serviceName() const { return serviceName_; }

    int requestLimit() const { return requestLimit_; }
    int requestPayloadLimit() const { return requestPayloadLimit_; }
    Pattern host() const { return host_; }
    Pattern uri() const { return uri_; }

    const HttpServerSecurity *security() const { return security_; }
    const MediaTypeDatabase *mediaTypeDatabase() const { return mediaTypeDatabase_; }

    const LogConfig *errorLogConfig() const { return errorLogConfig_; }
    const LogConfig *accessLogConfig() const { return accessLogConfig_; }

protected:
    DeliveryInstance(const MetaObject *config);
    ~DeliveryInstance();

    String serviceName_;

    int requestLimit_;
    int requestPayloadLimit_;
    Pattern host_;
    Pattern uri_;

    Ref<const HttpServerSecurity> security_;
    Ref<const MediaTypeDatabase> mediaTypeDatabase_;
    Ref<const LogConfig> errorLogConfig_;
    Ref<const LogConfig> accessLogConfig_;
};

typedef List< Ref<DeliveryInstance> > DeliveryInstances;

} // namespace ccnode
