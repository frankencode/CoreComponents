/*
 * Copyright (C) 2019-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/TokiStylePrototype>
#include <cc/TokiStyle>
#include <cc/Color>

namespace cc {

struct TokiStylePrototype::State final: public MetaPrototype::State
{
    State():
        MetaPrototype::State{"Style"}
    {}

    MetaObject produce() const
    {
        return TokiStyle{New{}};
    }
};

TokiStylePrototype::TokiStylePrototype():
    MetaPrototype{new State}
{
    establish("rule", "");
    establish("ink", Color{});
    establish("paper", Color{});
    establish("bold", false);
    establish("italic", false);
}

} // namespace cc
