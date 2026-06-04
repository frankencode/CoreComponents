/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/TokiStyle>

namespace cc {

void TokiStyle::State::realize()
{
    ruleName_ = value("rule").to<String>();
    ink_ = value("ink").to<Color>();
    paper_ = value("paper").to<Color>();
    bold_ = value("bold").to<bool>();
    italic_ = value("italic").to<bool>();
}

} // namespace cc
