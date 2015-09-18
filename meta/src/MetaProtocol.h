/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXMETA_METAPROTOCOL_H
#define FLUXMETA_METAPROTOCOL_H

#include <flux/meta/MetaObject>

namespace flux {
namespace meta {

/** \brief Duck-typed object protocol
  */
class MetaProtocol: public Object
{
public:
    inline static Ref<MetaProtocol> create() {
        return new MetaProtocol;
    }

    template<class Prototype>
    Prototype *define(const String &className) {
        Ref<Prototype> prototype = Prototype::create(className);
        define(prototype);
        return prototype;
    }

    template<class Prototype>
    Prototype *define() {
        Ref<Prototype> prototype = Prototype::create();
        define(prototype);
        return prototype;
    }

    MetaObject *define(MetaObject *prototype) {
        prototype->define();
        prototypes()->insert(prototype->className(), prototype);
        return prototype;
    }

    template<class Prototype>
    static Ref<MetaObject> createPrototype() {
        Ref<MetaObject> prototype = Prototype::create();
        prototype->define();
        return prototype;
    }

    virtual MetaObject *lookup(String className) const {
        MetaObject *prototype = 0;
        if (prototypes_) prototypes_->lookup(className, &prototype);
        return prototype;
    }

    int minCount() const { return minCount_; }
    int maxCount() const { return maxCount_; }
    void minCount(int newCount) { minCount_ = newCount; }
    void maxCount(int newCount) { maxCount_ = newCount; }

    inline bool lookup(const String &className, MetaObject **prototype) const {
        *prototype = lookup(className);
        return *prototype;
    }

protected:
    friend class YasonSyntax;

    MetaProtocol():
        minCount_(0),
        maxCount_(flux::intMax)
    {}

    virtual Ref<MetaObject> produce(MetaObject *prototype) const {
        return prototype->produce();
    }

private:
    typedef Map<String, Ref<MetaObject> > Prototypes;
    Prototypes *prototypes() {
        if (!prototypes_) prototypes_ = Prototypes::create();
        return prototypes_;
    }
    Ref<Prototypes> prototypes_;
    int minCount_;
    int maxCount_;
};

}} // namespace flux::meta

#endif // FLUXMETA_METAPROTOCOL_H
