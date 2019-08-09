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
#include <cc/http/HttpServerSecurity>
#include "ServiceDefinition.h"
#include "LogConfig.h"

namespace ccnode {

using namespace cc;
using namespace cc::net;
using namespace cc::http;

class NodeConfig: public Object
{
public:
    static Ref<NodeConfig> load(int argc, char **argv);
    static Ref<NodeConfig> load(const String &path);
    static Ref<NodeConfig> load(MetaObject *config);

    String directoryPath() const { return directoryPath_; }

    const SocketAddressList *address() const { return address_; }
    int securePort() const { return securePort_; }
    String user() const { return user_; }
    String group() const { return group_; }
    String version() const { return version_; }
    bool daemon() const { return daemon_; }
    int concurrency() const { return concurrency_; }
    int serviceWindow() const { return serviceWindow_; }
    int connectionLimit() const { return connectionLimit_; }
    double connectionTimeout() const { return connectionTimeout_; }

    const HttpServerSecurity *security() const { return securityConfig_; }
    const LogConfig *errorLogConfig() const { return errorLogConfig_; }
    const LogConfig *accessLogConfig() const { return accessLogConfig_; }

    ServiceInstances *serviceInstances() const { return serviceInstances_; }

    ServiceInstance *selectService(const String &host, const String &uri = "") const;

private:
    NodeConfig(MetaObject *config, const String &dirPath = "");

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

    Ref<HttpServerSecurity> securityConfig_;
    Ref<LogConfig> errorLogConfig_;
    Ref<LogConfig> accessLogConfig_;

    Ref<ServiceInstances> serviceInstances_;
};

} // namespace ccnode
