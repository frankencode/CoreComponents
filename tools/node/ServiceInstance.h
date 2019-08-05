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
#include "LogConfig.h"

namespace cc { namespace meta { class MetaObject; } }

namespace ccnode {

using namespace cc;
using namespace cc::glob;

class ServiceDelegate;
class ServiceWorker;
class SecurityConfig;

class ServiceInstance: public Object
{
public:
    virtual Ref<ServiceDelegate> createDelegate(ServiceWorker *worker) const = 0;

    String serviceName() const { return serviceName_; }

    int requestLimit() const { return requestLimit_; }
    int requestPayloadLimit() const { return requestPayloadLimit_; }
    Pattern host() const { return host_; }
    Pattern uri() const { return uri_; }

    SecurityConfig *security() const { return security_; }

    LogConfig *errorLogConfig() const { return errorLogConfig_; }
    LogConfig *accessLogConfig() const { return accessLogConfig_; }

protected:
    ServiceInstance(MetaObject *config);
    ~ServiceInstance();

    String serviceName_;

    int requestLimit_;
    int requestPayloadLimit_;
    Pattern host_;
    Pattern uri_;

    Ref<SecurityConfig> security_;
    Ref<LogConfig> errorLogConfig_;
    Ref<LogConfig> accessLogConfig_;
};

typedef List< Ref<ServiceInstance> > ServiceInstances;

} // namespace ccnode
