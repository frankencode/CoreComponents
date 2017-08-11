/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

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

    bool hasProtocol() const;
    MetaProtocol *protocol() { return protocol_; }
    const MetaProtocol *protocol() const { return protocol_; }

    inline bool hasChildren() const { return children_->count() > 0; }
    MetaObjectList *children() { return children_; }
    const MetaObjectList *children() const { return children_; }

    Ref<MetaObject> clone();

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
    Ref<MetaProtocol> protocol_;
    Ref<MetaObjectList> children_;
};

}} // namespace cc::meta
