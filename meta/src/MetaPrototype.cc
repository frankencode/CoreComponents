/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/meta/MetaPrototype>
#include <cc/meta/MetaProtocol>

namespace cc {
namespace meta {

Ref<MetaPrototype> MetaPrototype::create(const String &className, const MetaProtocol *protocol)
{
    return new MetaPrototype{className, protocol};
}

MetaPrototype::MetaPrototype(const String &className, const MetaProtocol *protocol):
    MetaObject{className},
    protocol_{protocol}
{
    if (!protocol_) protocol_ = MetaProtocol::create();
}

MetaPrototype::~MetaPrototype()
{}

}} // namespace cc::meta
