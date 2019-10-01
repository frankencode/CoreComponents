/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/meta/MetaObject>
#include <cc/meta/yason>
#include <cc/Guard>

namespace cc {
namespace meta {

Ref<MetaObject> MetaObject::create(const String &className)
{
    return new MetaObject{className};
}

MetaObject::MetaObject(const String &className):
    className_{className},
    children_{MetaObjectList::create()}
{}

MetaObject::~MetaObject()
{}

Variant MetaObject::toVariant() const
{
    return Ref<MetaObject>{const_cast<MetaObject *>(this)};
}

String MetaObject::toString() const
{
    return yason::stringify(toVariant());
}

Ref<MetaObject> MetaObject::clone() const
{
    auto object = produce();
    autocomplete(object);
    object->realize();
    return object;
}

void MetaObject::autocomplete(MetaObject *target) const
{
    if (count() != target->count()) {
        for (int i = 0; i < count(); ++i) {
            String name = keyAt(i);
            if (target->count() <= i || target->keyAt(i) != name) {
                Variant value = valueAt(i);
                const MetaProtocol *protocol = Variant::cast<const MetaProtocol *>(value);
                if (protocol) value = protocol->defaultObject();
                target->insert(name, value);
            }
        }
    }

    for (const MetaObject *child: children())
        target->children_->append(child->clone());
}

void MetaObject::realize()
{}

}} // namespace cc::meta
