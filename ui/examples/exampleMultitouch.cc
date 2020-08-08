#include <cc/ui/Application>
#include <cc/ui/DisplayManager>
#include <cc/ui/TouchDeviceManager>
#include <cc/ui/FingerEvent>
#include <cc/ui/PointerEvent>
#include <cc/Queue>
#include <cc/Map>
#include <cc/stdio>
#include <cc/DEBUG>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    if (TouchDeviceManager::instance()->getTouchDeviceCount() == 0) {
        ferr() << "No touch device found!" << nl;
        return 1;
    }

    Application app;
    app->cursorVisible = false;
    app->screenSaverEnabled = true;

    View view;
    view->size = DisplayManager::instance()->getDisplay(0)->nativeMode()->resolution();

    Queue<View> freeTouchPoints;
    {
        Ref<TouchDevice> touchDevice = TouchDeviceManager::instance()->getTouchDevice(0);

        Color contactColors[] = {
            { 0xFF, 0x00, 0x00, 0xFF },
            { 0x00, 0xFF, 0x00, 0xFF },
            { 0x00, 0x00, 0xFF, 0xFF },
            { 0xFF, 0xFF, 0x00, 0xFF },
            { 0x00, 0xFF, 0xFF, 0xFF },
            { 0xFF, 0x00, 0xFF, 0xFF },
            { 0xFF, 0x80, 0x80, 0xFF },
            { 0x80, 0xFF, 0x80, 0xFF },
            { 0x80, 0x80, 0xFF, 0xFF },
            { 0x80, 0x80, 0x80, 0xFF },
            { 0x00, 0x00, 0x00, 0xFF },
        };

        while (freeTouchPoints->count() < 10) {
            Color nextColor = contactColors[freeTouchPoints->count()];
            View touchPoint{100, 100};
            touchPoint->paper = Color::Transparent;
            touchPoint->paint <<[=]{
                Painter p{touchPoint};
                p->setSource(nextColor);
                p->circle(touchPoint->size() / 2, touchPoint->size()[0]/2 - 1);
                p->fill();
            };
            view << touchPoint;
            freeTouchPoints->pushBack(touchPoint);
        }
    }

    Map<TouchFingerId, View> assignedTouchPoints;
    auto onPressed = [&](const FingerEvent *event){
        View touchPoint { nullptr };
        if (!assignedTouchPoints->lookup(event->fingerId(), &touchPoint)) {
            freeTouchPoints->popFront(&touchPoint);
            assignedTouchPoints->establish(event->fingerId(), touchPoint);
        }
        // touchPoint->visible = true;
        touchPoint->pos = /*view->size() **/ event->pos() - touchPoint->size() / 2;
        return true;
    };
    auto onRelease = [&](const FingerEvent *event){
        View touchPoint { nullptr };
        if (assignedTouchPoints->lookup(event->fingerId(), &touchPoint)) {
            // touchPoint->visible = false;
            touchPoint->pos = Point{};
            freeTouchPoints->pushBack(touchPoint);
        }
        return true;
    };
    view->fingerPressed >> onPressed;
    view->fingerMoved >> onPressed;
    view->fingerReleased >> onRelease;

    Window::open(view, "Touch Test", WindowMode::Fullscreen|WindowMode::Accelerated|WindowMode::VSync);

    return app->run();
}
