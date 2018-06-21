#include <cc/System>
#include <cc/ui/Application>
#include <cc/ui/View>
#include <cc/ui/Timer>

using namespace cc;
using namespace cc::ui;

class MainView: public View
{
    friend class Object;

    MainView()
    {
        size = Size{640, 480};
        paper = 0xFFFFFF;
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
    auto app = Application::open(argc, argv);
    auto window = Window::open<MainView>();
    window->title->bind([=]{ return "Test window " + str(window->size()) + " at " + str(window->pos()); });
    return app->run();
}
