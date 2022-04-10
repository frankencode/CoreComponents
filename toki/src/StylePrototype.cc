/*
 * Copyright (C) 2019-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/toki/StylePrototype>
#include <cc/toki/Style>
#include <cc/Color>

namespace cc::toki {

struct StylePrototype::State final: public MetaPrototype::State
{
    State():
        MetaPrototype::State{"Style"}
    {}

    MetaObject produce() const
    {
        return Style{New{}};
    }
};

StylePrototype::StylePrototype():
    MetaPrototype{new State}
{
    establish("rule", "");
    establish("ink", Color{});
    establish("paper", Color{});
    establish("bold", false);
    establish("italic", false);
}

} // namespace cc::toki
