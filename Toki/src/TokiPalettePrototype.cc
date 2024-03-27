/*
 * Copyright (C) 2019-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
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
