/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Map>
#include <cc/String>
#include <cc/net/SocketAddress>
#include "ServiceDefinition.h"
#include "SecurityConfig.h"
#include "LogConfig.h"

namespace cc { template<class> class Singleton; }

namespace ccnode {

using namespace cc;
using namespace cc::net;

class NodeConfig: public Object
{
public:
    void load(int argc, char **argv);

    inline String directoryPath() const { return directoryPath_; }

    inline SocketAddressList *address() const { return address_; }
    inline int securePort() const { return securePort_; }
    inline String user() const { return user_; }
    inline String group() const { return group_; }
    inline String version() const { return version_; }
    inline bool daemon() const { return daemon_; }
    inline int concurrency() const { return concurrency_; }
    inline int serviceWindow() const { return serviceWindow_; }
    inline int connectionLimit() const { return connectionLimit_; }
    inline double connectionTimeout() const { return connectionTimeout_; }

    inline SecurityConfig *security() const { return securityConfig_; }
    inline LogConfig *errorLogConfig() const { return errorLogConfig_; }
    inline LogConfig *accessLogConfig() const { return accessLogConfig_; }

    inline ServiceInstances *serviceInstances() const { return serviceInstances_; }

    ServiceInstance *selectService(String host, String uri = "") const;

private:
    friend class Singleton<NodeConfig>;

    NodeConfig();

    String directoryPath_;

    Ref<SocketAddressList> address_;
    String user_;
    String group_;
    String version_;
    bool daemon_;
    int concurrency_;
    int serviceWindow_;
    int connectionLimit_;
    double connectionTimeout_;
    int securePort_;

    Ref<SecurityConfig> securityConfig_;
    Ref<LogConfig> errorLogConfig_;
    Ref<LogConfig> accessLogConfig_;

    Ref<ServiceInstances> serviceInstances_;
};

NodeConfig *nodeConfig();

} // namespace ccnode
