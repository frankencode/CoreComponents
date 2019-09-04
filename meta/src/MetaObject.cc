/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Guard>
#include <cc/meta/YasonSyntax>
#include <cc/meta/MetaProtocol>
#include <cc/meta/MetaObject>

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
    object->MetaObject::autocomplete(this);
    if (hasChildren()) {
        for (const MetaObject *child: children()) {
            object->children_->append(child->clone());
        }
    }
    return object;
}

void MetaObject::autocomplete(const MetaObject *prototype) // FIXME: add name suffix "By"
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

Token *MetaObject::nameToken(const CharArray *text, Token *objectToken, const String &memberName)
{
    return YasonSyntax::instance()->nameToken(text, objectToken, memberName);
}

Token *MetaObject::valueToken(const CharArray *text, Token *objectToken, const String &memberName)
{
    return YasonSyntax::instance()->valueToken(text, objectToken, memberName);
}

Token *MetaObject::childToken(Token *objectToken, int childIndex)
{
    return YasonSyntax::instance()->childToken(objectToken, childIndex);
}

}} // namespace cc::meta
