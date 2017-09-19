/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Queue>
#include <cc/ui/Font>

namespace cc {
namespace ui {

class PaintCommand: public Object
{
public:
    enum Type {
        FillRect,
        DrawText
    };

    inline Type type() const { return type_; }

protected:
    PaintCommand(Type type):
        type_(type)
    {}

private:
    Type type_;
};

typedef Queue< Ref<PaintCommand> > PaintQueue;

}} // namespace cc::ui

