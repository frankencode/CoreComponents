/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Array>
#include <cc/Map>
#include <cc/IoTarget>
#include <cc/SystemIo>

namespace cc {

/** \class IoEvent IoMonitor.h cc/IoMonitor.cc
  * \brief I/O event
  */
class IoEvent: public Object
{
public:
    inline int type() const { return type_; }
    inline int ready() const { return ready_; }
    inline IoTarget *target() const { return target_; } // FIXME: media

private:
    friend class IoMonitor;

    inline static Ref<IoEvent> create(int index, int type, IoTarget *target) {
        return new IoEvent(index, type, target);
    }

    IoEvent(int index, int type, IoTarget *target):
        index_(index),
        type_(type),
        ready_(0),
        target_(target)
    {}

    int index_;
    int type_;
    int ready_;
    IoTarget *target_;
};

typedef Array<const IoEvent *> IoActivity;

/** \class IoMonitor IoMonitor.h cc/IoMonitor.cc
  * \brief Register and wait for I/O events
  */
class IoMonitor: public Object // FIXME: IoWatcher
{
public:
    static Ref<IoMonitor> create(int maxCount = 0);

    const IoEvent *addEvent(int type, IoTarget *target); // FIXME: "watch"
    void removeEvent(const IoEvent *event); // FIXME: "unwatch"

    Ref<IoActivity> poll(int interval_ms = -1); // FIXME: "wait", "timeout_ms" (vs. time!-)

private:
    typedef struct pollfd PollFd;
    typedef Array<PollFd> Fds;
    typedef Map<int, Ref<IoEvent> > Events;

    IoMonitor(int maxCount);

    Ref<Fds> fds_;
    Ref<Events> events_;
};

} // namespace cc
