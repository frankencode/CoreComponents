#include <cc/stdio>
#include <cc/debug>
#include <cc/System>
#include <cc/ui/Application>
#include <cc/ui/Control>
#include <cc/ui/PointerEvent>
#include <cc/ui/WheelEvent>
#include <cc/ui/FingerEvent>

using namespace cc;
using namespace cc::ui;

class MainView: public Control
{
    friend class Object;

    MainView()
    {
        size = Size{640, 480};
        paper = 0xFFFFFF;
    }

    void paint() override
    {
        Painter p(this);

        p->translate(size()/2);

        p->setSource(Color{0xFF0000}(50));
        p->circle(-Point{30, 0}, 60);
        p->fill();

        p->setSource(Color{0x00FF00}(50));
        p->circle(Point{0, 0}, 60);
        p->fill();

        p->setSource(Color{0x0000FF}(50));
        p->circle(Point{30, 0}, 60);
        p->fill();
    }

    bool onPointerPressed(const PointerEvent *event) override
    {
        fout() << "Pressed at " << event->pos() << nl;
        return true;
    }

    bool onPointerReleased(const PointerEvent *event) override
    {
        fout() << "Released at " << event->pos() << nl;
        return true;
    }

    bool onPointerClicked(const PointerEvent *event) override
    {
        fout() << "Clicked at " << event->pos() << nl;
        return true;
    }

    bool onPointerMoved(const PointerEvent *event) override
    {
        fout("Moved to %% (%%, %%)\n") << event->pos() << event->time() << System::now();
        return true;
    }
};

int main(int argc, char **argv)
{
    auto app = Application::open(argc, argv);
    Window::open<MainView>("Hello, world!", WindowMode::Accelerated|WindowMode::VSync);
    return app->run();
}
