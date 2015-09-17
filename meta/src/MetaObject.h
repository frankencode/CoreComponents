/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXMETA_METAOBJECT_H
#define FLUXMETA_METAOBJECT_H

#include <flux/SpinLock>
#include <flux/Variant>

namespace flux {

class MetaProtocol;

namespace syntax { class Token; }
using flux::syntax::Token;

class MetaObject;
typedef List< Ref<MetaObject> > MetaObjectList;

/** \brief Duck-typed object
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

} // namespace flux

#endif // FLUXMETA_METAOBJECT_H
