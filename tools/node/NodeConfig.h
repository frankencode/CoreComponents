/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_NODECONFIG_H
#define FLUXNODE_NODECONFIG_H

#include <flux/Map>
#include <flux/String>
#include <flux/net/SocketAddress>
#include "ServiceDefinition.h"
#include "LogConfig.h"

namespace flux { template<class> class Singleton; }

namespace fluxnode {

using namespace flux;
using namespace flux::net;

class NodeConfig: public Object
{
public:
    void load(int argc, char **argv);

    inline String directoryPath() const { return directoryPath_; }

    inline SocketAddressList *address() const { return address_; }
    inline String user() const { return user_; }
    inline String version() const { return version_; }
    inline bool daemon() const { return daemon_; }
    inline int serviceWindow() const { return serviceWindow_; }

    inline LogConfig *errorLogConfig() const { return errorLogConfig_; }
    inline LogConfig *accessLogConfig() const { return accessLogConfig_; }

    inline ServiceInstances *serviceInstances() const { return serviceInstances_; }

private:
    friend class Singleton<NodeConfig>;

    NodeConfig();

    String directoryPath_;

    Ref<SocketAddressList> address_;
    String user_;
    String version_;
    bool daemon_;
    int serviceWindow_;

    Ref<LogConfig> errorLogConfig_;
    Ref<LogConfig> accessLogConfig_;

    Ref<ServiceInstances> serviceInstances_;
};

NodeConfig *nodeConfig();

} // namespace fluxnode

#endif // FLUXNODE_NODECONFIG_H
