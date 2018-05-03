#include <cc/stdio>
#include <cc/debug>
#include <cc/System>
#include <cc/ui/Application>
#include <cc/ui/View>
#include <cc/ui/PointerEvent>
#include <cc/ui/WheelEvent>
#include <cc/ui/FingerEvent>

using namespace cc;
using namespace cc::ui;

class MainView: public View
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

        p->setSource(0xFF000080);
        p->circle(-Point{30, 0}, 60);
        p->fill();

        p->setSource(0x00FF0080);
        p->circle(Point{0, 0}, 60);
        p->fill();

        p->setSource(0x0000FF80);
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
};

int main(int argc, char **argv)
{
    Application *app = Application::open(argc, argv);
    Window::open(Object::create<MainView>(), "Hello, world!", WindowMode::InputFocus);
    return app->run();
}
