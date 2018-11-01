/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/assert>
#include <cc/exceptions>
#include <cc/IoMonitor>

namespace cc {

Ref<IoMonitor> IoMonitor::create(int maxCount) { return new IoMonitor(maxCount); }

IoMonitor::IoMonitor(int maxCount):
    fds_(Fds::create(maxCount)),
    events_(Events::create())
{}

const IoEvent *IoMonitor::addEvent(IoReady type, IoTarget *target)
{
    CC_ASSERT(events_->count() < fds_->count());
    Ref<IoEvent> event = IoEvent::create(events_->count(), type, target);
    events_->insert(event->index_, event);
    PollFd *p = &fds_->at(event->index_);
    p->fd = target->fd();
    p->events = static_cast<short>(type);
    return event;
}

void IoMonitor::removeEvent(const IoEvent *event)
{
    int i = event->index_;
    int n = events_->count();

    if (i != n - 1) {
        IoEvent *h = events_->value(n - 1);
        h->index_ = i;
        events_->establish(i, h);
        fds_->at(i) = fds_->at(n - 1);
    }

    events_->remove(n - 1);
}

Ref<IoActivity> IoMonitor::wait(int timeout_ms)
{
    PollFd *fds = 0;
    if (events_->count() > 0) fds = fds_->data();
    int n = -1;
    do n = ::poll(fds, events_->count(), timeout_ms < 0 ? -1 : timeout_ms);
    while (n == -1 && errno == EINTR);
    if (n < 0) CC_SYSTEM_DEBUG_ERROR(errno);

    CC_ASSERT(n <= events_->count());

    Ref<IoActivity> activity = IoActivity::create(n);
    int j = 0;
    for (int i = 0; i < events_->count(); ++i) {
        if (fds_->at(i).revents != 0) {
            CC_ASSERT(j < n);
            IoEvent *event = events_->value(i);
            event->ready_ = static_cast<IoReady>(fds_->at(i).revents & (POLLIN|POLLOUT));
            activity->at(j) = event;
            ++j;
        }
    }

    CC_ASSERT(j == n);

    return activity;
}

bool IoMonitor::waitFor(const IoEvent *event, int timeout_ms, const IoEvent **other)
{
    PollFd *fds = 0;
    if (events_->count() > 0) fds = fds_->data();
    int n = -1;
    do n = ::poll(fds, events_->count(), timeout_ms < 0 ? -1 : timeout_ms);
    while (n == -1 && errno == EINTR);
    if (n < 0) CC_SYSTEM_DEBUG_ERROR(errno);

    if (n > 0) {
        for (int i = 0; i < events_->count(); ++i) {
            const IoEvent *candidate = events_->valueAt(i);
            if (candidate == event) {
                if (fds_->at(i).revents != 0) return true;
                if (!other) return false;
            }
            if (other) {
                if (fds_->at(i).revents != 0)
                    *other = candidate;
            }
        }
    }

    return false;
}

} // namespace cc
