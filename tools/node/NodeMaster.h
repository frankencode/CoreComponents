/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Thread>

namespace cc {
    template<class> class Singleton;
    class SignalMaster;
}

namespace ccnode {

using namespace cc;

class NodeMaster: public Thread
{
public:
    static int run(int argc, char **argv);

private:
    friend class Singleton<NodeMaster>;

    NodeMaster();

    virtual void run();
    void runNode() const;

    Ref<SignalMaster> signalMaster_;
    int exitCode_;
};

} // namespace ccnode

