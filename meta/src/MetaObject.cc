/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Guard>
#include <cc/syntax/Token>
#include <cc/meta/YasonSyntax>
#include <cc/meta/MetaProtocol>
#include <cc/meta/MetaObject>

namespace cc {
namespace meta {

MetaObject::MetaObject(const String &className, MetaProtocol *protocol)
    : className_(className),
      protocol_(protocol)
{}

Variant MetaObject::toVariant() const
{
    return Ref<MetaObject>(const_cast<MetaObject *>(this));
}

String MetaObject::toString() const
{
    return yason::stringify(toVariant());
}

MetaObjectList *MetaObject::children()
{
    if (!children_) {
        Guard<SpinLock> guard(&mutex_);
        if (!children_) children_ = MetaObjectList::create();
    }
    return children_;
}

MetaProtocol *MetaObject::protocol()
{
    if (!protocol_) {
        Guard<SpinLock> guard(&mutex_);
        if (!protocol_) protocol_ = MetaProtocol::create();
    }
    return protocol_;
}

Ref<MetaObject> MetaObject::clone()
{
    Ref<MetaObject> object = produce();
    object->MetaObject::autocomplete(this);
    if (hasChildren()) {
        for (int i = 0; i < children()->count(); ++i) {
            object->children()->append(children()->at(i)->clone());
        }
    }
    return object;
}

void MetaObject::autocomplete(const MetaObject *prototype)
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

Token *MetaObject::nameToken(const ByteArray *text, Token *objectToken, const String &memberName)
{
    return YasonSyntax::instance()->nameToken(text, objectToken, memberName);
}

Token *MetaObject::valueToken(const ByteArray *text, Token *objectToken, const String &memberName)
{
    return YasonSyntax::instance()->valueToken(text, objectToken, memberName);
}

Token *MetaObject::childToken(Token *objectToken, int childIndex)
{
    return YasonSyntax::instance()->childToken(objectToken, childIndex);
}

}} // namespace cc::meta
