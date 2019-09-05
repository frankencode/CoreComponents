/*
 * Copyright (C) 2007-2017 Frank Mertens.
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

bool MetaObject::hasProtocol() const { return protocol_->isDefined(); }

Ref<MetaObject> MetaObject::clone() const
{
    Ref<MetaObject> object = produce();
    object->MetaObject::autocompleteBy(this);
    if (hasChildren()) {
        for (const MetaObject *child: children()) {
            object->children_->append(child->clone());
        }
    }
    return object;
}

void MetaObject::autocompleteBy(const MetaObject *prototype)
{
    if (!prototype) return;

    if (prototype->count() != count()) {
        for (int i = 0; i < prototype->count(); ++i) {
            String name = prototype->keyAt(i);
            if (count() <= i || keyAt(i) != name)
                insert(name, prototype->valueAt(i));
        }
    }
}

}} // namespace cc::meta
