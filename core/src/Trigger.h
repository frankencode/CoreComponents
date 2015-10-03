#ifndef FLUX_TRIGGER_H
#define FLUX_TRIGGER_H

#include <flux/Object>
#include <flux/Map>
#include <flux/Set>
#include <flux/List>

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
    inline void reverseConnect(ConnectionEndPoint *trigger, Recipient *recipient);
    inline void reverseDisconnect(ConnectionEndPoint *trigger, Recipient *recipient);
};

template<class Value>
class Trigger: public ConnectionEndPoint
{
public:
    inline static Ref<Trigger> create() { return new Trigger; }

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

    Trigger() {}

    Ref<CallbackListByRecipient> callbacks_;
};

class Recipient
{
protected:
    Recipient():
        triggers_(Triggers::create())
    {}

    ~Recipient() {
        for (int i = 0; i < triggers_->count(); ++i)
            triggers_->at(i)->disconnect(this);
    }

private:
    friend class ConnectionEndPoint;

    typedef Set< Ref<ConnectionEndPoint> > Triggers;
    Ref<Triggers> triggers_;
};

inline void ConnectionEndPoint::reverseConnect(ConnectionEndPoint *signal, Recipient *recipient) {
    recipient->triggers_->insert(signal);
}
inline void ConnectionEndPoint::reverseDisconnect(ConnectionEndPoint *signal, Recipient *recipient) {
    recipient->triggers_->remove(signal);
}

} // namespace flux

#endif // FLUX_TRIGGER_H
