/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/meta/MetaObject>
#include <cc/meta/MetaProtocol>
#include <cc/meta/yason>
#include <cc/Guard>

namespace cc {
namespace meta {

Ref<MetaObject> MetaObject::create(const String &className)
{
    return new MetaObject{className};
}

MetaObject::MetaObject(const String &className, const MetaProtocol *protocol):
    className_{className},
    protocol_{protocol},
    children_{MetaObjectList::create()}
{
    if (!protocol_) protocol_ = MetaProtocol::create();
}

MetaObject::~MetaObject()
{}

Variant MetaObject::toVariant() const
{
    return Ref<MetaObject>(const_cast<MetaObject *>(this));
}

String MetaObject::toString() const
{
    return yason::stringify(toVariant());
}

bool MetaObject::hasProtocol() const
{
    return protocol_->isDefined();
}

Ref<MetaObject> MetaObject::clone() const
{
    Ref<MetaObject> object = produce();
    autocomplete(object);
    if (hasChildren()) {
        for (const MetaObject *child: children()) {
            object->children_->append(child->clone());
        }
    }
    object->realize();
    return object;
}

void MetaObject::autocomplete(MetaObject *target) const
{
    if (count() != target->count()) {
        for (int i = 0; i < count(); ++i) {
            String name = keyAt(i);
            if (target->count() <= i || target->keyAt(i) != name)
                target->insert(name, valueAt(i));
        }
    }
}

void MetaObject::realize()
{}

}} // namespace cc::meta
