/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/debug>
#include <cc/Singleton>
#include <cc/File>
#include <cc/ResourceGuard>
#include <cc/toki/PaletteLoader>

namespace cc {
namespace toki {

const PaletteLoader *PaletteLoader::instance()
{
    return Singleton<PaletteLoader>::instance();
}

PaletteLoader::PaletteLoader():
    protocol_{MetaProtocol::create()}
{
    protocol_->define<Palette>();
    protocol_->minCount(1);
    protocol_->maxCount(1);
}

Ref<Palette> PaletteLoader::load(const String &path) const
{
    ResourceGuard context{path};
    String text = File::open(path)->map();
    return yason::parse(text, protocol_);
}

}} // namespace cc::toki
