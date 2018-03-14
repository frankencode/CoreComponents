#include <cc/stdio>
#include <cc/debug>
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
            { 0x00, 0x00, 0x00, 0xFF }
        };

        const int n = 10;
        for (int i = 0; i < n; ++i) {
            Object::create<TouchPoint>(this)
                ->fgColor = fgColors[i % 7];
        }
    }

    void touchEvent(const TouchEvent *event) override
    {
        // CC_DEBUG << event->pos() << ", " << event->pressure();

        const int n = touchDevice_->getTouchFingerCount();
        const int m = childCount();

        for (int i = 0; i < m; ++i) {
            View *child = childAt(i);
            if (i < n) {
                TouchFinger finger = touchDevice_->getTouchFinger(i);
                child->pos = size() * finger->pos() - child->size() / 2;
                child->visible = true;
            }
            else
                child->visible = false;
        }
    }

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
