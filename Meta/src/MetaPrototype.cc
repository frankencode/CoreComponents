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

const TypeInfo VariantType<MetaPrototype>::info
{
    .typeName = "MetaPrototype",
    .str = [](const void *bytes) {
        //! \TODO
        return String{};
    },
    .cleanup = [](void *bytes) {
        static_cast<MetaPrototype *>(bytes)->~MetaPrototype();
    },
    .assign = [](void *dst, const void *src) {
        new(dst)MetaPrototype{*static_cast<const MetaPrototype *>(src)};
    }
};

} // namespace cc
