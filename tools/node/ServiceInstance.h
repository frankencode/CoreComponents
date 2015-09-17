/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_SERVICEINSTANCE_H
#define FLUXNODE_SERVICEINSTANCE_H

#include <flux/List>
#include <flux/syntax/RegExp>
#include "LogConfig.h"

namespace flux { class MetaObject; }

namespace fluxnode {

using namespace flux;
using namespace flux::syntax;

class ServiceDelegate;
class ServiceWorker;

class ServiceInstance: public Object
{
public:
    virtual Ref<ServiceDelegate> createDelegate(ServiceWorker *worker) const = 0;

    inline String serviceName() const { return serviceName_; }

    inline int concurrency() const { return concurrency_; }
    inline double connectionTimeout() const { return connectionTimeout_; }
    inline RegExp host() const { return host_; }
    inline RegExp uri() const { return uri_; }

    inline LogConfig *errorLogConfig() const { return errorLogConfig_; }
    inline LogConfig *accessLogConfig() const { return accessLogConfig_; }
    inline LogConfig *visitLogConfig() const { return visitLogConfig_; }

protected:
    ServiceInstance(MetaObject *config);

    String serviceName_;

    int concurrency_;
    double connectionTimeout_;
    RegExp host_;
    RegExp uri_;

    Ref<LogConfig> errorLogConfig_;
    Ref<LogConfig> accessLogConfig_;
    Ref<LogConfig> visitLogConfig_;
};

typedef List< Ref<ServiceInstance> > ServiceInstances;

} // namespace fluxnode

#endif // FLUXNODE_SERVICEINSTANCE_H
