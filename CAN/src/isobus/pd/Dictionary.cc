/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/isobus/pd/Dictionary>
#include <cc/isobus/pd/TypeInfoSource>
#include <cc/Format>

namespace cc::isobus::pd {

struct Dictionary::State final: public Object::State
{
    explicit State(const String &text)
    {
        for (TypeInfo typeInfo: TypeInfoSource{text}) {
             if (Dictionary::isProprietary(typeInfo.typeId())) continue;

            if (!typeInfos_.insert(typeInfo.typeId(), typeInfo)) {
                throw TextError{text, typeInfo.offset(), Format{"Redefinition of type: %%"} << typeInfo.typeId()};
            }
        }
    }

    Map<uint32_t, TypeInfo> typeInfos_;
};

Dictionary::Dictionary(const String &text):
    Object{new State{text}}
{}

bool Dictionary::isProprietary(uint32_t typeId)
{
    return 57344 <= typeId && typeId <= 65534;
}

bool Dictionary::lookup(uint32_t typeId, Out<TypeInfo> typeInfo) const
{
    return me().typeInfos_.lookup(typeId, typeInfo);
}

Map<uint32_t, TypeInfo> Dictionary::typeInfos() const
{
    return me().typeInfos_;
}

const Dictionary::State &Dictionary::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::isobus::pd
