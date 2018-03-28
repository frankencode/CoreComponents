#include <cc/System>
#include <cc/ui/Application>
#include <cc/ui/Timer>

using namespace cc;
using namespace cc::ui;

class TestView: public View
{
    friend class Object;

    TestView()
    {
        size = Size{640, 480};
        color = Color{"#FFFFFF"};
        /*double f = 60;
        double t0 = System::now();
        Timer::start(1/f, [=]{
            double t = System::now();
            angle = f * (t - t0) * 2;
            update();
        });*/
    }

    void paint() override
    {
        Painter p(this);

        p->translate(center());

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
};

int main(int argc, char **argv)
{
    Application *app = Application::open(argc, argv);
    Window *window = Window::open(Object::create<TestView>(), "Hello, world!");
    window->title->bind([=]{ return "Test window " + str(window->size()) + " at " + str(window->pos()); });
    return app->run();
}
