/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Object>
#include <cc/Map>
#include <cc/Set>
#include <cc/List>

namespace cc {

template<class Value>
class Callback: public Object {
public:
    virtual void invoke(Value value) = 0;
};

template<class Recipient, class Value>
class Slot: public Callback<Value>
{
public:
    typedef void (Recipient::* Method)(Value);

    Slot():
        recipient_(0),
        method_(0)
    {}

    Slot(Recipient *recipient, Method method):
        recipient_(recipient),
        method_(method)
    {}

    inline void invoke(Value value) { (recipient_->*method_)(value); }

private:
    Recipient *recipient_;
    Method method_;
};

class Recipient;

class ConnectionEndPoint: public Object
{
protected:
    friend class Recipient;

    virtual void disconnect(Recipient *recipient) = 0;
    inline void reverseConnect(ConnectionEndPoint *trigger, Recipient *recipient);
    inline void reverseDisconnect(ConnectionEndPoint *trigger, Recipient *recipient);
};

template<class Value>
class Event: public ConnectionEndPoint
{
public:
    inline static Ref<Event> create() { return new Event; }

    ~Event() {
        disconnectAll();
    }

    template<class Recipient>
    void connect(Recipient *recipient, void (Recipient::* method)(Value))
    {
        if (!callbacks_)
            callbacks_ = CallbackListByRecipient::create();

        Ref<CallbackList> list;
        if (!callbacks_->lookup(recipient, &list)) {
            list = CallbackList::create();
            callbacks_->insert(recipient, list);
        }

        list->append(new Slot<Recipient, Value>(recipient, method));
        reverseConnect(this, recipient);
    }

    void disconnect(Recipient *recipient)
    {
        if (!callbacks_) return;

        if (callbacks_->remove(recipient));
        reverseDisconnect(this, recipient);
    }

    void disconnectAll()
    {
        if (!callbacks_) return;

        for (int i = 0; i < callbacks_->count(); ++i)
            reverseDisconnect(this, callbacks_->keyAt(i));

        callbacks_ = 0;
    }

    void notify(Value value)
    {
        if (!callbacks_) return;

        for (int i = 0; i < callbacks_->count(); ++i) {
            CallbackList *list = callbacks_->valueAt(i);
            for (int j = 0; j < list->count(); ++j)
                list->at(j)->invoke(value);
        }
    }

private:
    typedef List< Ref< Callback<Value> > > CallbackList;
    typedef Map< Recipient *, Ref<CallbackList> > CallbackListByRecipient;

    Event() {}

    Ref<CallbackListByRecipient> callbacks_;
};

class Recipient
{
protected:
    Recipient():
        events_(Events::create())
    {}

    ~Recipient() {
        while (events_->count() > 0)
            events_->at(0)->disconnect(this);
    }

private:
    friend class ConnectionEndPoint;

    typedef Set<ConnectionEndPoint *> Events;
    Ref<Events> events_;
};

inline void ConnectionEndPoint::reverseConnect(ConnectionEndPoint *event, Recipient *recipient) {
    recipient->events_->insert(event);
}
inline void ConnectionEndPoint::reverseDisconnect(ConnectionEndPoint *event, Recipient *recipient) {
    recipient->events_->remove(event);
}

} // namespace cc
