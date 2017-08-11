/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/meta/MetaObject>

namespace cc {
namespace meta {

/** \class MetaProtocol MetaProtocol.h cc/meta/MetaProtocol
  * \brief Duck-typed object protocol
  */
class MetaProtocol: public Object
{
public:
    inline static Ref<MetaProtocol> create() {
        return new MetaProtocol;
    }
    
    bool isDefined() const { return prototypes_->count() > 0; }

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
        prototypes_->insert(prototype->className(), prototype);
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
        prototypes_(Prototypes::create()),
        minCount_(0),
        maxCount_(cc::intMax)
    {}

    virtual Ref<MetaObject> produce(MetaObject *prototype) const {
        return prototype->produce();
    }

private:
    typedef Map<String, Ref<MetaObject> > Prototypes;
    Ref<Prototypes> prototypes_;
    int minCount_;
    int maxCount_;
};

}} // namespace cc::meta
