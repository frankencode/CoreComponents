/*
 * Copyright (C) 2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/CanErrorAnnotator>
#include <cc/CanError>

namespace cc {

struct CanErrorAnnotator::State final: public CanAnnotator::State
{
    String annotate(const CanFrame &frame) override
    {
        return CanError{frame} ? CanError{frame}.toString() : String{};
    }
};

CanErrorAnnotator::CanErrorAnnotator():
    CanAnnotator{new State}
{}

} // namespace cc
