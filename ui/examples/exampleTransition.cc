#include <cc/stdio>
#include <cc/ui/Application>
#include <cc/ui/View>
#include <cc/ui/Transition>

using namespace cc;
using namespace cc::ui;

class MainView: public View
{
    friend class Object;

    MainView()
    {
        size = Size{640, 480};
        color = Color{"#FFFFFF"};

        // easeOn(angle, 0.5, easing::outElastic);
        easeOn(angle, 0.5, easing::Bezier(0.5, -0.4, 0.5, 1.4));
    }

    void paint() override
    {
        Painter p(this);

        p->translate(center());
        p->setSource(Color{"#0080FFFF"});
        p->rectangle(-Point{50, 50}, Size{100, 100});
        p->fill();
    }

    bool hasPointerInput() const override { return true; }

    bool onPointerClicked(const PointerEvent *event) override
    {
        fout() << "Clicked at " << event->pos() << nl;
        angle += 45;
        return true;
    }
};

int main(int argc, char **argv)
{
    Application *app = Application::open(argc, argv);
    Window::open(Object::create<MainView>(), "Click me!", WindowMode::Default|WindowMode::Accelerated);
    return app->run();
}