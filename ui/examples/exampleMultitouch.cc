#include <cc/stdio>
#include <cc/debug>
#include <cc/CircularBuffer>
#include <cc/ui/DisplayManager>
#include <cc/ui/TouchDeviceManager>
#include <cc/ui/Application>

using namespace cc;
using namespace cc::ui;

class TestView: public View
{
    friend class Object;

    typedef CircularBuffer<Color> FreeColors;

    TestView():
        touchDevice_(TouchDeviceManager::instance()->getTouchDevice(0)),
        freeColors_(FreeColors::create(10))
    {
        size = DisplayManager::instance()->getDisplay(0)->nativeMode()->resolution();
        color = Color{"#FFFFFF"};

        {
            const int n = freeColors_->capacity();
            for (int i = 0; i < n; ++i)
                freeColors_->pushBack(Color::fromHue(i * 360 / n));
        }

        for (int i = 0; i < freeColors_->count(); ++i)
            CC_INSPECT(freeColors_->at(i));
    }

    void touchEvent(const TouchEvent *event) override
    {
        // CC_DEBUG << event->pos() << ", " << event->pressure();
        update();
    }

    void paint() override
    {
        Painter p(this);
        int n = touchDevice_->getTouchFingerCount();

        // CC_DEBUG << n;
        for (int i = 0; i < n; ++i) {
            TouchFinger finger = touchDevice_->getTouchFinger(i);
            // CC_DEBUG << finger->pos() << ": " << finger->pressure();
            p->setSource(freeColors_->at(i));
            p->circle(size() * finger->pos(), 60);
            p->fill();
        }
    }

    Ref<TouchDevice> touchDevice_;
    Ref<FreeColors> freeColors_;
};

int main(int argc, char **argv)
{
    if (TouchDeviceManager::instance()->getTouchDeviceCount() > 0) {
        ferr() << "No touch device found!" << nl;
        return 1;
    }
    Application *app = Application::open(argc, argv);
    Window *window = Window::open(Object::create<TestView>(), argv[0], WindowMode::Fullscreen);
    return app->run();
}
