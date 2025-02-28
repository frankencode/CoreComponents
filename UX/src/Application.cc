/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Application>
#include <cc/TimeMaster>
#include <cc/TimerState>
#include <cc/PlatformPlugin>
#include <cc/PosGuard>
#include <cc/InputControl>
#include <cc/Process>
#include <cc/Bmp>
#include <cc/stdio>

namespace cc {

void Application::State::notifyTimer(const Timer &timer)
{
    if (!timer) return;
    if (timer.interval() > 0) {
        TimeMaster{}.ack();
    }
    timer.me().timeout_.emit();
}

Application::State::State()
{
    orgName("CoreComponents");
    minMouseDragDistance([]{ return dp(6); });
    minFingerDragDistance([]{ return dp(10); });

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

void Application::State::initAppName(const char *exePath)
{
    if (appName() == "") {
        appName(String{exePath}.baseName());
    }
}

String Application::State::userDataPath()
{
    if (userDataPath_ == "") {
        userDataPath_ = getUserDataPath();
    }
    return userDataPath_;
}

bool Application::State::feedFingerEvent(const Window &window, FingerEvent &event)
{
    if (cursorControl()) cursorControl = Control{};
    if (hoverControl()) hoverControl = Control{};

    showCursor(false);

    Point eventPos = window.size() * event.pos();
    Control topControl = window.findControl(eventPos);

    if (topControl) {
        topControl.me().pointerPos = topControl.mapToLocal(eventPos);
    }

    if (event.action() == PointerAction::Moved)
    ;
    else if (event.action() == PointerAction::Pressed)
    {
        if (!pressedControl()) {
            pressedControl = topControl;
        }
    }
    else if (event.action() == PointerAction::Released)
    {
        if (focusControl() != pressedControl()) {
            focusControl = Control{};
        }
    }

    bool eaten = false;

    if (pressedControl())
    {
        eaten = pressedControl().me().feedFingerEvent(event);

        if (event.action() == PointerAction::Released)
        {
            PosGuard<> guard{event, pressedControl().mapToLocal(eventPos)};

            if (
                pressedControl().me().onPointerClicked(event) ||
                pressedControl().me().onFingerClicked(event)
            ) {
                eaten = true;
            }

            pressedControl = Control{};
        }
    }

    if (!eaten) {
        eaten = window.view().me().feedFingerEvent(event);
    }

    return eaten;
}

bool Application::State::feedMouseEvent(const Window &window, MouseEvent &event)
{
    Control topControl = window.findControl(event.pos());

    if (topControl) {
        topControl.me().pointerPos = topControl.mapToLocal(event.pos());
    }

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
        if (focusControl() != pressedControl()) {
            focusControl = Control{};
        }
        hoverControl = topControl;
    }

    bool eaten = false;

    if (pressedControl())
    {
        eaten = pressedControl().me().feedMouseEvent(event);

        if (event.action() == PointerAction::Released)
        {
            PosGuard<> guard{event, pressedControl().mapToLocal(event.pos())};

            if (
                pressedControl().me().onPointerClicked(event) ||
                pressedControl().me().onMouseClicked(event)
            ) {
                eaten = true;
            }

            pressedControl = Control{};
            captureMouse(false);
        }
    }

    if (!eaten) {
        eaten = window.view().me().feedMouseEvent(event);
    }

    cursorControl = topControl;
    showCursor(true);

    return eaten;
}

bool Application::State::feedPenEvent(const Window &window, PenEvent &event)
{
    // showCursor(false);
    return window.view().me().feedPenEvent(event);
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

    Image image{view.size()};

    String path = Format{"%%.bmp"}.arg(fixed(System::now(), 0));
    view.renderTo(image);

    Bmp::save(path, image);

    ferr() << "Written screenshot to file://" << Process::cwd() << "/" << path << nl;
}

void Application::State::disengage(const View &view)
{
    if (view.isParentOf(hoverControl())) {
        hoverControl(Control{});
    }
    if (view.isParentOf(pressedControl())) {
        pressedControl(Control{});
    }
    if (view.isParentOf(focusControl())) {
        focusControl(Control{});
    }
    if (view.isParentOf(cursorControl())) {
        cursorControl(Control{});
    }
    if (view.isParentOf(focusControlSaved_)) {
        focusControlSaved_ = Control{};
    }
}

Application::Application()
{
    *this = platform().application();
}

Application::Application(int argc, char *argv[])
{
    *this = platform().application();
    me().initAppName(argv[0]);
    me().init(argc, argv);
}

String Application::orgName() const
{
    return me().orgName();
}

Application &Application::orgName(const String &newValue)
{
    me().orgName(newValue);
    return *this;
}

String Application::appName() const
{
    return me().appName();
}

Application &Application::appName(const String &newValue)
{
    me().appName(newValue);
    return *this;
}

String Application::userDataPath()
{
    return me().userDataPath();
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

void Application::postEvent(Function<void()> &&doNext)
{
    me().postEvent(move(doNext));
}

Application app()
{
    return platform().application();
}

} // namespace cc
