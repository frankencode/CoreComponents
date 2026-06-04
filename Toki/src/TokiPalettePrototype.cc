/*
 * Copyright (C) 2019-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/TokiPalettePrototype>
#include <cc/TokiPalette>
#include <cc/TokiStylePrototype>

namespace cc {

struct TokiPalettePrototype::State final: public MetaPrototype::State
{
    State():
        MetaPrototype::State{
            "Palette",
            [](){
                MetaProtocol protocol;
                protocol.define(TokiStylePrototype{});
                protocol.setMinCount(1);
                return protocol;
            }()
        }
    {}

    MetaObject produce() const
    {
        return TokiPalette{New{}};
    }
};

TokiPalettePrototype::TokiPalettePrototype():
    MetaPrototype{new State}
{}

} // namespace cc
