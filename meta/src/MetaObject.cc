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

Ref<MetaObject> MetaObject::create(const string &className)
{
    return new MetaObject{className};
}

MetaObject::MetaObject(const string &className):
    className_{className},
    children_{MetaObjectList::create()}
{}

MetaObject::~MetaObject()
{}

variant MetaObject::toVariant() const
{
    return Ref<MetaObject>{const_cast<MetaObject *>(this)};
}

string MetaObject::toString() const
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
            string name = at(i)->key();
            if (target->count() <= i || target->at(i)->key() != name) {
                variant value = at(i)->value();
                const MetaProtocol *protocol = variant::cast<const MetaProtocol *>(value);
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
