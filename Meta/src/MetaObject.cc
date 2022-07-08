/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/MetaObject>
#include <cc/MetaPrototype>
#include <cc/YasonWriter>
#include <cc/CaptureSink>

namespace cc {

void MetaObject::autocomplete(InOut<MetaObject> target) const
{
    if (members().count() != target().members().count()) {
        for (int i = 0, n = members().count(); i < n; ++i) {
            String name = members().at(i).key();
            if (target().members().count() <= i || target().members().at(i).key() != name) {
                Variant value = members().at(i).value();
                if (value.is<MetaProtocol>()) value = value.to<MetaProtocol>().defaultObject();
                else if (value.is<MetaPrototype>()) value = value.to<MetaPrototype>().as<MetaObject>();
                target().members().insert(name, value);
            }
        }
    }

    target().children().appendList(children());
}

MetaObject MetaObject::clone() const
{
    auto object = produce();
    autocomplete(&object);
    object.me().realize();
    return object;
}

const TypeInfo VariantType<MetaObject>::info
{
    .typeName = "MetaObject",
    .str = [](const void *bytes) {
        CaptureSink sink;
        YasonWriter{sink}.writeObject(*static_cast<const MetaObject *>(bytes));
        return sink.collect();
    },
    .cleanup = [](void *bytes) {
        static_cast<MetaObject *>(bytes)->~MetaObject();
    },
    .assign = [](void *dst, const void *src) {
        new(dst)MetaObject{*static_cast<const MetaObject *>(src)};
    },
    .equal = [](const void *a, const void *b) {
        return VariantType<MetaObject>::retrieve(a) == VariantType<MetaObject>::retrieve(b);
    }
};

} // namespace cc
