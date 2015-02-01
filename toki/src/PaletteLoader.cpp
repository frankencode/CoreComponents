/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Singleton>
#include <flux/File>
#include <flux/ResourceGuard>
#include <flux/toki/PaletteLoader>

namespace flux {
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

}} // namespace flux::toki
