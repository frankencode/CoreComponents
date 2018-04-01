#include <cc/stdio>
#include <cc/debug>
#include <cc/System>
#include <cc/ui/Application>
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
        color = Color{"#FFFFFF"};
    }

    void paint() override
    {
        Painter p(this);

        p->translate(size()/2);

        p->setSource(Color{"#FF000080"});
        p->circle(-Point{30, 0}, 60);
        p->fill();

        p->setSource(Color{"#00FF0080"});
        p->circle(Point{0, 0}, 60);
        p->fill();

        p->setSource(Color{"#0000FF80"});
        p->circle(Point{30, 0}, 60);
        p->fill();
    }

    bool hasPointerInput() const override { return true; }

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
    Window::open(Object::create<MainView>(), "Hello, world!", WindowMode::Default|WindowMode::InputFocus);
    return app->run();
}
