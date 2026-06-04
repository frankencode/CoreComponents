/*
 * Copyright (C) 2026 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/ContentLayout>
#include <cc/Window>

namespace cc {

struct ContentLayout::State final: public View::Layout::State
{
    State() = default;

    Orientation orientation() const override { return Orientation::Vertical; }

    void updateLayout();

    Property<void> update { [this]{ updateLayout(); } };
};

void ContentLayout::State::updateLayout()
{
    if (!hasView() || !view().hasWindow() || view().childrenCount() == 0) return;

    view().size(view().children().first().size());
}

ContentLayout::ContentLayout():
    Layout{new ContentLayout::State}
{}

ContentLayout::State& ContentLayout::me()
{
    return Object::me.as<State>();
}

const ContentLayout::State& ContentLayout::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
