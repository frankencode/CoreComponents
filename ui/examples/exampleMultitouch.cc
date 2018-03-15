#include <cc/stdio>
#include <cc/debug>
#include <cc/CircularBuffer>
#include <cc/Map>
#include <cc/ui/DisplayManager>
#include <cc/ui/TouchDeviceManager>
#include <cc/ui/Application>

using namespace cc;
using namespace cc::ui;

class TouchPoint: public View
{
    friend class Object;

public:
    Property<Color> fgColor;

protected:
    TouchPoint(View *parent):
        View(parent)
    {
        color = transparent;
        size = Size{100, 100};
    }

    void paint() override
    {
        Painter p(this);
        p->setSource(fgColor());
        p->circle(size() / 2, size()[0]/2 - 1);
        p->fill();
    }
};

class TestView: public View
{
    friend class Object;

    TestView():
        assignedTouchPoints_(AssignedTouchPoints::create()),
        freeTouchPoints_(FreeTouchPoints::create(10)),
        touchDevice_(TouchDeviceManager::instance()->getTouchDevice(0))
    {
        size = DisplayManager::instance()->getDisplay(0)->nativeMode()->resolution();
        color = Color{"#FFFFFF"};

        Color fgColors[] = {
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

        while (freeTouchPoints_->count() < freeTouchPoints_->capacity()) {
            Ref<TouchPoint> touchPoint = Object::create<TouchPoint>(this);
            touchPoint->fgColor = fgColors[freeTouchPoints_->count()];
            freeTouchPoints_->pushBack(touchPoint);
        }
    }

    void touchEvent(const TouchEvent *event) override
    {
        if (event->action() == TouchAction::Pressed || event->action() == TouchAction::Moved)
        {
            Ref<TouchPoint> touchPoint;
            if (!assignedTouchPoints_->lookup(event->fingerId(), &touchPoint)) {
                touchPoint = freeTouchPoints_->popFront();
                assignedTouchPoints_->establish(event->fingerId(), touchPoint);
            }
            touchPoint->visible = true;
            touchPoint->pos = size() * event->pos() - touchPoint->size() / 2;
        }
        else if (event->action() == TouchAction::Released)
        {
            Ref<TouchPoint> touchPoint;
            if (assignedTouchPoints_->lookup(event->fingerId(), &touchPoint)) {
                touchPoint->visible = false;
                freeTouchPoints_->pushBack(touchPoint);
            }
        }
    }

    typedef Map<TouchFingerId, TouchPoint *> AssignedTouchPoints;
    typedef CircularBuffer<TouchPoint *> FreeTouchPoints;

    Ref<AssignedTouchPoints> assignedTouchPoints_;
    Ref<FreeTouchPoints> freeTouchPoints_;
    Ref<TouchDevice> touchDevice_;
};

int main(int argc, char **argv)
{
    if (TouchDeviceManager::instance()->getTouchDeviceCount() > 0) {
        ferr() << "No touch device found!" << nl;
        return 1;
    }
    Application *app = Application::open(argc, argv);
    app->cursorVisible = false;
    app->screenSaverEnabled = true;

    Window *window = Window::open(Object::create<TestView>(), argv[0], WindowMode::Fullscreen);

    return app->run();
}
