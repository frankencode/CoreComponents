/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_PROPERTY_H
#define FLUX_PROPERTY_H

#include <flux/Object>
#include <flux/Format>
#include <flux/Map>
#include <flux/Set>

namespace flux {

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
    inline void reverseConnect(ConnectionEndPoint *signal, Recipient *recipient);
    inline void reverseDisconnect(ConnectionEndPoint *signal, Recipient *recipient);
};

template<class Value>
class Signal: public ConnectionEndPoint
{
public:
    inline static Ref<Signal> create() { return new Signal; }

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

    void emit(Value value)
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

    Signal() {}

    Ref<CallbackListByRecipient> callbacks_;
};

template<class T>
class Property
{
public:
    Property() {}
    Property(const T &b): value_(b) {}

    inline Property &operator=(const T &b) {
        if (b != value_) {
            value_ = b;
            if (valueChanged_)
                valueChanged_->emit(value_);
        }
        return *this;
    }

    inline operator const T&() const { return value_; }

    inline Signal<T> *valueChanged() {
        if (!valueChanged_) valueChanged_ = Signal<T>::create();
        return valueChanged_;
    }

    inline Property *operator->() { return this; }
    inline const Property *operator->() const { return this; }

private:
    Ref< Signal<T> > valueChanged_;
    T value_;
};

class Recipient
{
protected:
    Recipient():
        signals_(Signals::create())
    {}

    ~Recipient() {
        for (int i = 0; i < signals_->count(); ++i)
            signals_->at(i)->disconnect(this);
    }

private:
    friend class ConnectionEndPoint;

    typedef Set< Ref<ConnectionEndPoint> > Signals;
    Ref<Signals> signals_;
};

inline void ConnectionEndPoint::reverseConnect(ConnectionEndPoint *signal, Recipient *recipient) {
    recipient->signals_->insert(signal);
}
inline void ConnectionEndPoint::reverseDisconnect(ConnectionEndPoint *signal, Recipient *recipient) {
    recipient->signals_->remove(signal);
}

} // namespace flux

#endif // FLUX_PROPERTY_H
