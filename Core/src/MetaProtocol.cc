/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/MetaProtocolState>

namespace cc {

MetaProtocol::MetaProtocol(New):
    Object{new State}
{}

MetaProtocol::MetaProtocol(State *newState):
    Object{newState}
{}

void MetaProtocol::inherit(const MetaProtocol &parent)
{
    me().parent_ = parent;
}

void MetaProtocol::define(const MetaPrototype &prototype)
{
    me().define(prototype);
}

const Map<String, MetaPrototype> &MetaProtocol::prototypes() const
{
    if (!Object::me) {
        thread_local Map<String, MetaPrototype> dummy;
        return dummy;
    }
    return me().prototypes_;
}

long MetaProtocol::minCount() const
{
    return (Object::me) ? me().minCount_ : 0;
}

long MetaProtocol::maxCount() const
{
    return (Object::me) ?  me().maxCount_ : 0;
}

void MetaProtocol::setMinCount(long newValue)
{
    me().minCount_ = newValue;
}

void MetaProtocol::setMaxCount(long newValue)
{
    me().maxCount_ = newValue;
}

bool MetaProtocol::lookup(const String &className, Out<MetaPrototype> prototype) const
{
    return (Object::me) ? me().lookup(className, prototype) : false;
}

MetaObject MetaProtocol::defaultObject() const
{
    return (Object::me) ? me().defaultObject_ : MetaObject{};
}

void MetaProtocol::setDefaultObject(const MetaObject &object)
{
    me().defaultObject_ = object;
}

const MetaProtocol::State &MetaProtocol::me() const
{
    return Object::me.as<State>();
}

MetaProtocol::State &MetaProtocol::me()
{
    if (!Object::me) Object::me = new State;
    return Object::me.as<State>();
}

/** \internal
  * \todo implement str()
  */
struct MetaProtocolTypeInfo final: public TypeInfo
{
    const char *typeName() const { return "MetaProtocol"; }

    String str(const void *bytes) const override { return {}; }

    void cleanup(void *bytes) const override { static_cast<MetaProtocol *>(bytes)->~MetaProtocol(); }

    void assign(void *dst, const void *src) const override { new(dst)MetaProtocol{*static_cast<const MetaProtocol *>(src)}; }
};

const TypeInfo &VariantType<MetaProtocol>::info()
{
    return global<MetaProtocolTypeInfo>();
}

} // namespace cc
