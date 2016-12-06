/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/SpinLock>
#include <cc/Variant>

namespace cc { namespace syntax { class Token; } }

namespace cc {
namespace meta {

using namespace cc::syntax;

class MetaProtocol;

class MetaObject;
typedef List< Ref<MetaObject> > MetaObjectList;

/** \class MetaObject MetaObject.h cc/meta/MetaObject
  * \brief Duck-typed object
  */
class MetaObject: public VariantMap
{
public:
    inline static Ref<MetaObject> create(const String &className = "", MetaProtocol *protocol = 0) {
        return new MetaObject(className, protocol);
    }

    inline String className() const { return className_; }
    Variant toVariant() const;
    String toString() const;

    inline bool hasChildren() const { return children_; }
    MetaObjectList *children();

    Ref<MetaObject> clone();

    inline bool hasProtocol() const { return protocol_; }
    MetaProtocol *protocol();

    virtual Ref<MetaObject> produce() {
        return MetaObject::create(className());
    }

    virtual void autocomplete(const MetaObject *prototype);

    virtual void realize(const ByteArray *text, Token *objectToken) {}

protected:
    friend class MetaProtocol;

    MetaObject(const String &className = "", MetaProtocol *protocol = 0);

    inline void className(String newName) { className_ = newName; }
    virtual void define() {}

    static Token *nameToken(const ByteArray *text, Token *objectToken, const String &memberName);
    static Token *valueToken(const ByteArray *text, Token *objectToken, const String &memberName);
    static Token *childToken(Token *objectToken, int childIndex);

private:
    String className_;
    SpinLock mutex_;
    Ref<MetaObjectList> children_;
    Ref<MetaProtocol> protocol_;
};

}} // namespace cc::meta
