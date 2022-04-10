/*
 * Copyright (C) 2019-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/toki/PalettePrototype>
#include <cc/toki/Palette>
#include <cc/toki/StylePrototype>

namespace cc::toki {

struct PalettePrototype::State final: public MetaPrototype::State
{
    State():
        MetaPrototype::State{
            "Palette",
            [](){
                MetaProtocol protocol;
                protocol.define(StylePrototype{});
                protocol.setMinCount(1);
                return protocol;
            }()
        }
    {}

    MetaObject produce() const
    {
        return Palette{New{}};
    }
};

PalettePrototype::PalettePrototype():
    MetaPrototype{new State}
{}

} // namespace cc::toki
