/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Window>
#include <cc/Application>
#include <cc/DisplayManager>

namespace cc {

Window::Window(const View &view)
{
    *this = Application{}->createWindow(view);
}

Control Window::findControl(Point pos) const
{
    Control control;
    if (view()) {
        if (view().is<Control>()) {
            control = view().as<Control>();
            if (control.delegate()) {
                while (control.delegate()) {
                    control = control.delegate();
                }
            }
            else if (control.cascade()) {
                control = control.findControl(pos);
            }
        }
        else {
            control = view().findControl(pos);
        }
    }
    return control;
}

Window::State::State(View view):
    size{&view->size},
    view_{view}
{
    if (!view_.paper()) view_.paper([this]{ return theme().windowColor(); });
    view_->window_ = this;
    view_->settled();

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

int Window::run()
{
    show();
    return Application{}.run();
}

} // namespace cc
