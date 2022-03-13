/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/IoMonitor>
#include <cc/Map>
#include <poll.h>

namespace cc {

struct IoMonitor::State: public Object::State
{
    Map<int, IoActivity> subjects;
    Array<struct pollfd> fds;
    bool dirty { true };
};

IoMonitor::IoMonitor():
    Object{new State}
{}

void IoMonitor::watch(const IoStream &target, IoEvent mask)
{
    me().dirty = true;
    me().subjects.establish(target.fd(), IoActivity{target, mask});
}

void IoMonitor::unwatch(const IoStream &target)
{
    me().dirty = true;
    me().subjects.remove(target.fd());
}

bool IoMonitor::wait(const Call<void(const IoActivity &)> &onReady, int timeout)
{
    assert(me().subjects.count() > 0);

    if (me().dirty) {
        me().dirty = false;
        me().fds = Array<struct pollfd>::allocate(me().subjects.count());
        long i = 0;
        for (const auto &pair: me().subjects) {
            struct pollfd &p = me().fds[i++];
            p.fd = pair.value().target().fd();
            p.events = 0;
            if (pair.value().mask() & IoEvent::ReadyRead) p.events |= POLLIN;
            if (pair.value().mask() & IoEvent::ReadyWrite) p.events |= POLLOUT;
            if (pair.value().mask() & IoEvent::Established) p.events = POLLIN|POLLOUT;
        }
    }

    int n = -1;
    do n = ::poll(me().fds, me().fds.count(), (timeout < 0) ? -1 : timeout);
    while (n == -1 && errno == EINTR);
    if (n < 0) CC_SYSTEM_DEBUG_ERROR(errno);

    bool ready = (n > 0);

    if (ready) {
        for (int i = 0; i < me().fds.count(); ++i) {
            const struct pollfd &p = me().fds[i];
            if (p.revents != 0) {
                IoActivity &activity = me().subjects(p.fd);
                IoEvent &event = activity.me().event_;
                event = IoEvent::None;
                if (p.revents & POLLIN) event |= IoEvent::ReadyRead;
                if (p.revents & POLLOUT) event |= IoEvent::ReadyWrite;
                onReady(activity);
                if (--n == 0) break;
            }
        }
    }

    return ready;
}

IoMonitor::State &IoMonitor::me()
{
    return Object::me.as<State>();
}

const IoMonitor::State &IoMonitor::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
