/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Application>
#include <cc/TimeMaster>
#include <cc/PlatformPlugin>
#include <cc/PosGuard>
#include <cc/InputControl>
#include <cc/Process>
#include <cc/ImageFile>
#include <cc/stdio>

namespace cc {

void Application::State::notifyTimer(const Timer &timer)
{
    if (timer.interval() > 0) {
        TimeMaster{}.ack();
    }
    timer.me().timeout_.emit();
}

Application::State::State()
{
    textInputArea.onChanged([this]{
        if (focusControl())
            setTextInputArea(textInputArea());
    });

    focusControl.onChanged([this]{
        if (focusControlSaved_) stopTextInput();
        focusControlSaved_ = focusControl();

        if (focusControl()) {
            startTextInput(focusControl().window());

            textInputArea([this]{
                if (!focusControl()) return Rect{};
                Rect a = focusControl().me().textInputArea();
                return Rect{focusControl().mapToGlobal(a.pos()), a.size()};
            });

            setTextInputArea(textInputArea());
        }
        else
            textInputArea = Rect{};
    });

    cursorControl.onChanged([this]{
        if (cursorControl()) {
            cursor([this]{
                return cursorControl() ? cursorControl().cursor() : Cursor{};
            });
        }
        else {
            cursor = Cursor{};
        }
    });

    cursor.onChanged([this]{
        if (cursor()) setCursor(cursor());
        else unsetCursor();
    });
}

bool Application::State::feedFingerEvent(const Window &window, FingerEvent &event)
{
    cursorControl = Control{};
    hoverControl = Control{};

    if (event.action() == PointerAction::Pressed) {
        Point pos = window.size() * event.pos();
        Control topControl = window.findControl(window.view().mapToLocal(pos));
        if (topControl) {
            touchTargets_.establish(event.fingerId(), topControl);
            pressedControl = topControl;
        }
    }

    if (event.action() == PointerAction::Released) {
        if (focusControl() != pressedControl())
            focusControl = Control{};
    }

    Control touchTarget;
    if (touchTargets_.lookup(event.fingerId(), &touchTarget)) {
        if (event.action() == PointerAction::Released)
            touchTargets_.remove(event.fingerId());
    }

    if (pressedControl()) {
        Point pos = window.size() * event.pos();
        pressedControl().me().pointerPos = pressedControl().mapToLocal(pos);
        bool eaten = pressedControl().me().feedFingerEvent(event);
        if (event.action() == PointerAction::Released) {
            PosGuard guard{event, pressedControl().mapToLocal(window.size() * event.pos())};
            if (pressedControl().containsLocal(event.pos())) {
                if (
                    pressedControl().me().onPointerClicked.propagate(event) ||
                    pressedControl().me().onFingerClicked.propagate(event)
                )
                    eaten = true;
            }
            pressedControl = Control{};
        }
        if (eaten) return true;
    }
    else if (touchTarget) {
        if (touchTarget.me().feedFingerEvent(event))
            return true;
    }

    return window.view().me().feedFingerEvent(event);
}

bool Application::State::feedMouseEvent(const Window &window, MouseEvent &event)
{
    Control topControl = window.findControl(window.view().mapToLocal(event.pos()));

    if (topControl)
        topControl.me().pointerPos = topControl.mapToLocal(event.pos());

    if (event.action() == PointerAction::Moved)
    {
        hoverControl = topControl;
    }
    else if (event.action() == PointerAction::Pressed)
    {
        if (!pressedControl()) {
            pressedControl = topControl;
            if (topControl) captureMouse(true);
        }

        hoverControl = Control{};
    }
    else if (event.action() == PointerAction::Released)
    {
        if (focusControl() != pressedControl())
            focusControl = Control{};

        hoverControl = topControl;
    }

    bool eaten = false;

    if (pressedControl())
    {
        eaten = pressedControl().me().feedMouseEvent(event);

        if (event.action() == PointerAction::Released)
        {
            PosGuard guard{event, pressedControl().mapToLocal(event.pos())};

            if (
                pressedControl().me().onPointerClicked(event) ||
                pressedControl().me().onMouseClicked(event)
            )
                eaten = true;

            pressedControl = Control{};
            captureMouse(false);
        }
    }

    if (!eaten)
        eaten = window.view().me().feedMouseEvent(event);

    cursorControl = topControl;

    return eaten;
}

bool Application::State::feedWheelEvent(const Window &window, WheelEvent &event)
{
    bool eaten = false;
    if (hoverControl()) eaten = hoverControl().me().feedWheelEvent(event);
    if (!eaten) eaten = window.view().me().feedWheelEvent(event);
    return eaten;
}

bool Application::State::feedKeyEvent(const Window &window, KeyEvent &event)
{
    if (
        event.scanCode() == ScanCode::Key_ScrollLock &&
        event.action() == KeyAction::Pressed
    ) {
        takeScreenshot(window);
        return true;
    }

    if (focusControl()) {
        return focusControl().me().feedKeyEvent(event);
    }

    if (
        event.scanCode() == ScanCode::Key_Tab &&
        event.action() == KeyAction::Pressed &&
        !(event.modifiers() & KeyModifier::Shift) &&
        !(event.modifiers() & KeyModifier::Alt) &&
        !(event.modifiers() & KeyModifier::Control)
    ) {
        List<InputControl> inputControls;
        window.view().collectVisible<InputControl>(&inputControls);
        if (inputControls.count() > 0) {
            inputControls.mutableAt(0).focus(true);
        }
    }

    return true;
}

bool Application::State::feedExposedEvent(const Window &window)
{
    return window.view().me().feedExposedEvent();
}

bool Application::State::feedEnterEvent(const Window &window)
{
    return window.view().me().feedEnterEvent();
}

bool Application::State::feedLeaveEvent(const Window &window)
{
    hoverControl = Control{};
    return window.view().me().feedLeaveEvent();
}

bool Application::State::feedTextEditingEvent(const String &text, int start, int length)
{
    if (focusControl()) {
        focusControl().me().onTextEdited(text, start, length);
        return true;
    }

    return false;
}

bool Application::State::feedTextInputEvent(const String &text)
{
    if (focusControl()) {
        focusControl().me().onTextInput(text);
        return true;
    }

    return false;
}

void Application::State::takeScreenshot(const Window &window)
{
    View view = window.view();

    Image image{
        static_cast<int>(std::ceil(view.width())),
        static_cast<int>(std::ceil(view.height()))
    };

    String path = Format{"%%.bmp"}.arg(fixed(System::now(), 0));
    view.renderTo(image);

    ImageFile::save(path, image);

    ferr() << "Written screenshot to file://" << Process::cwd() << "/" << path << nl;
}

Application::Application()
{
    *this = platform().application();
}

Application::Application(int argc, char *argv[])
{
    *this = platform().application();
    me().init(argc, argv);
}

FontSmoothing Application::fontSmoothing() const
{
    if (me().fontSmoothing_ == FontSmoothing::Default)
        return DisplayManager{}.defaultFontSmoothing();

    return me().fontSmoothing_;
}

bool Application::pointerIsDragged(const PointerEvent &event, Point dragStart) const
{
    double minDragDistance = event.is<MouseEvent>() ? minMouseDragDistance() : minFingerDragDistance();
    return (event.pos() - dragStart).absPow2() >= minDragDistance * minDragDistance;
}

void Application::postEvent(Call<void()> &&doNext)
{
    me().postEvent(std::move(doNext));
}

Application app()
{
    return platform().application();
}

} // namespace cc
