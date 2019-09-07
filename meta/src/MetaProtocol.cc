/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/meta/MetaProtocol>

namespace cc {
namespace meta {

Ref<MetaProtocol> MetaProtocol::create()
{
    return new MetaProtocol;
}

MetaProtocol::MetaProtocol():
    prototypes_{Prototypes::create()},
    minCount_{0},
    maxCount_{cc::intMax}
{}

void MetaProtocol::define(const MetaPrototype *prototype)
{
    prototypes_->insert(prototype->className(), prototype);
}

const MetaPrototype *MetaProtocol::lookup(const String &className) const
{
    const MetaPrototype *prototype = nullptr;
    prototypes_->lookup(className, &prototype);
    return prototype;
}

bool MetaProtocol::lookup(const String &className, const MetaPrototype **prototype) const
{
    *prototype = lookup(className);
    return *prototype;
}

Ref<MetaObject> MetaProtocol::produce(const MetaPrototype *prototype) const
{
    return prototype->produce();
}

}} // namespace cc::meta
