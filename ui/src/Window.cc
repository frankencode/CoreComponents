/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Window>
#include <cc/Application>
#include <cc/DisplayManager>
#include <cc/DEBUG>

namespace cc {

Window::Window(const View &view)
{
    *this = Application{}->createWindow(view);
}

Control Window::findControl(Point pos) const
{
    if (view()) {
        Control control = view().findControl(pos);
        if (!control && view().containsLocal(pos) && view().is<Control>()) {
            control = view().as<Control>();
        }
        return control;
    }
    return Control{};
}

Window::State::State(View view):
    size{&view.me().size},
    view_{view}
{
    if (!view_.paper()) view_.paper([this]{ return theme().windowColor(); });
    view_.me().window_ = this;

    display([this]{
        List<Display> displays = DisplayManager{}.displays();
        Display displayFound;
        Point windowCenter = pos() + size() / 2;
        for (const Display &candidate: displays) {
            if (candidate.geometry().contains(windowCenter)) {
                displayFound = candidate;
                break;
            }
        }
        return displayFound;
    });
}

Window::State::~State()
{
    if (view_) view_.detachAll();
}

void Window::State::addToFrame(const UpdateRequest &request)
{
    if (nextFrame_.count() > 0) {
        if (nextFrame_.last() == request)
            return;
    }
    nextFrame_.pushBack(request);
}

void Window::State::commitFrame()
{
    if (nextFrame_.count() == 0) return;
    renderFrame(nextFrame_);
    nextFrame_ = Frame{};
}

} // namespace cc
