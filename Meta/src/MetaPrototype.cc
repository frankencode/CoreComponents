/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/MetaPrototype>

namespace cc {

MetaPrototype::MetaPrototype(const String &className):
    MetaObject{new State{className}}
{}

MetaPrototype::MetaPrototype(const String &className, const MetaProtocol &protocol):
    MetaObject{new State{className, protocol}}
{}

MetaPrototype::MetaPrototype(State *newState):
    MetaObject{newState}
{}

const MetaProtocol &MetaPrototype::protocol() const
{
    return me().protocol_;
}

const MetaPrototype::State &MetaPrototype::me() const
{
    return Object::me.as<State>();
}

/** \internal
  * \todo implement str()
  */
struct MetaPrototypeTypeInfo final: public TypeInfo
{
    const char *typeName() const override { return "MetaPrototype"; }

    String str(const void *bytes) const override { return {}; }

    void cleanup(void *bytes) const override { static_cast<MetaPrototype *>(bytes)->~MetaPrototype(); }

    void assign(void *dst, const void *src) const override { new(dst)MetaPrototype{*static_cast<const MetaPrototype *>(src)}; }
};

const TypeInfo &VariantType<MetaPrototype>::info()
{
    return global<MetaPrototypeTypeInfo>();
}

} // namespace cc
