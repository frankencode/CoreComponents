/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Thread>
#include "NodeConfig.h"

namespace cc { template<class> class Channel; }

namespace ccnode {

using namespace cc;

class NodeConfig;

class NodeMaster: public Thread
{
public:
    static Ref<NodeMaster> create(const NodeConfig *config);

    void signaled(Signal signal);

    int exitCode() const;

private:
    NodeMaster(const NodeConfig *config);

    const NodeConfig *config() const { return config_; }

    void run() final;
    void runNode();

    Ref<const NodeConfig> config_;

    typedef Channel<Signal> Signals;
    Ref<Signals> signals_;
    int exitCode_;
};

} // namespace ccnode
