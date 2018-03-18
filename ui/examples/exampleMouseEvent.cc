#include <cc/stdio>
#include <cc/debug>
#include <cc/System>
#include <cc/ui/Application>
#include <cc/ui/MouseEvent>
#include <cc/ui/MouseWheelEvent>
#include <cc/ui/TouchEvent>

using namespace cc;
using namespace cc::ui;

class TestView: public View
{
    friend class Object;

    TestView()
    {
        size = Size{640, 480};
        color = Color{"#FFFFFF"};

        pressed->connect([=]{ fout() << "pressed at " << mousePos() << nl; });
        released->connect([=]{ fout() << "released at " << mousePos() << nl; });
        clicked->connect([=]{ fout() << "clicked at " << mousePos() << nl; });
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

    #if 0
    void mouseEvent(const MouseEvent *event) override
    {
        CC_INSPECT(event);
    }

    void mouseWheelEvent(const MouseWheelEvent *event) override
    {
        CC_INSPECT(event);
    }

    void touchEvent(const TouchEvent *event) override
    {
        CC_INSPECT(event);
    }
    #endif

    double angle_;
};

int main(int argc, char **argv)
{
    Application *app = Application::open(argc, argv);
    Window::open(Object::create<TestView>(), "Hello, world!", WindowMode::Default|WindowMode::InputFocus);
    return app->run();
}
