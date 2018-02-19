#include <cc/stdio>
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
        angle_ = 0;
        double f = 60;
        double t0 = System::now();
        Timer::start(1/f, [=]{
            double t = System::now();
            angle_ = f * (t - t0) * degrees(2);
            update();
        });
    }

    void paint() override
    {
        Painter p(this);

        p->translate(size()/2);
        p->rotate(angle_);

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

    double angle_;
};

int main(int argc, char **argv)
{
    Application *app = Application::open(argc, argv);
    Window *window = Window::open(Object::create<TestView>(), "Hello, world!");
    window->title->bind([=]{ return String("Test " + str(window->pos())); });
    window->pos->connect([=]{ fout() << "Moved to " << window->pos() << nl; });
    window->size->connect([=]{ fout() << "Resized to " << window->size() << nl; });
    return app->run();
}
