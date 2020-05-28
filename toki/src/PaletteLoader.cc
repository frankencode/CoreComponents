/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/toki/PaletteLoader>
#include <cc/toki/PalettePrototype>
#include <cc/Singleton>
#include <cc/ResourceGuard>
#include <cc/File>

namespace cc {
namespace toki {

const PaletteLoader *PaletteLoader::instance()
{
    return Singleton<PaletteLoader>::instance();
}

PaletteLoader::PaletteLoader():
    protocol_{[]{
        auto protocol = MetaProtocol::create();
        protocol->define(PalettePrototype::create());
        protocol->setMinCount(1);
        protocol->setMaxCount(1);
        return protocol;
    }()}
{}

Ref<Palette> PaletteLoader::load(const String &path) const
{
    ResourceGuard context{path};
    String text = File::open(path)->map();
    return yason::parse(text, protocol_);
}

}} // namespace cc::toki
