/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Singleton>
#include <cc/File>
#include <cc/ResourceGuard>
#include <cc/toki/PaletteLoader>

namespace cc {
namespace toki {

PaletteLoader::PaletteLoader():
    protocol_(MetaProtocol::create())
{
    protocol_->define<Palette>();
    protocol_->minCount(1);
    protocol_->maxCount(1);
}

Ref<Palette> PaletteLoader::load(String path) const
{
    ResourceGuard context(path);
    String text = File::open(path)->map();
    return yason::parse(text, protocol_);
}

const PaletteLoader *paletteLoader() { return Singleton<PaletteLoader>::instance(); }

}} // namespace cc::toki
