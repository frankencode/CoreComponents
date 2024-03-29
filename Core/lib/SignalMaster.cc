/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/SignalMaster>
#include <cc/Thread>
#include <cc/SystemError>

namespace cc {

struct SignalMaster::State: public Object::State
{
    State(const Channel<Signal> &signals, const SignalSet &shutdownSet):
        signals_{signals},
        shutdownSet_{shutdownSet}
    {}

    void start()
    {
        thread_ = Thread{[this]{ run(); }};
        thread_.start();
    }

    void wait()
    {
        thread_.wait();
    }

    void kill(Signal signal)
    {
        thread_.kill(signal);
    }

    void run()
    {
        SignalSet all = SignalSet::full();
        while (true) {
            int signal = 0;
            int error = ::sigwait(all, &signal);
            if (error != 0) CC_SYSTEM_DEBUG_ERROR(error);
            signals_.pushBack(static_cast<Signal>(signal));
            if (shutdownSet_.contains(static_cast<Signal>(signal))) break;
        }
    }

    Channel<Signal> signals_;
    SignalSet shutdownSet_;
    Thread thread_;
};

SignalSet SignalMaster::defaultShutdownSignals()
{
    SignalSet set = SignalSet::empty();
    set.insert(Signal::Interrupt);
    set.insert(Signal::Terminate);
    return set;
}

SignalMaster::SignalMaster(const Channel<Signal> &signals, const SignalSet &shutdownSet):
    Object{new State{signals, shutdownSet}}
{}

void SignalMaster::start()
{
    me().start();
}

void SignalMaster::wait()
{
    me().wait();
}

void SignalMaster::kill(Signal signal)
{
    me().kill(signal);
}

SignalMaster::State &SignalMaster::me()
{
    return Object::me.as<State>();
}

} // namespace cc
