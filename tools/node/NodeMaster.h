/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Thread>
#include <cc/Channel>

namespace cc { template<class> class Singleton; }

namespace ccnode {

using namespace cc;

class NodeMaster: public Thread
{
public:
    static int run(int argc, char **argv);

private:
    friend class Singleton<NodeMaster>;

    NodeMaster();

    virtual void run() final;
    void runNode();

    typedef Channel<Signal> Signals;
    Ref<Signals> signals_;
    int exitCode_;
};

} // namespace ccnode
