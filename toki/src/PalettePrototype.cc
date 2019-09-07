/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/toki/PalettePrototype>
#include <cc/toki/StylePrototype>
#include <cc/toki/Palette>

namespace cc {
namespace toki {

Ref<PalettePrototype> PalettePrototype::create()
{
    return new PalettePrototype;
}

PalettePrototype::PalettePrototype():
    MetaPrototype{
        "Palette",
        [](){
            auto protocol = MetaProtocol::create();
            protocol->define<StylePrototype>();
            protocol->minCount(1);
            return protocol;
        }()
    }
{}

Ref<MetaObject> PalettePrototype::produce() const
{
    return Palette::create();
}

}} // namespace cc::toki
