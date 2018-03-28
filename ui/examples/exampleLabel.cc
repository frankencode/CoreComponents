#include <cc/debug>
#include <cc/System>
#include <cc/Date>
#include <cc/ui/Application>
#include <cc/ui/Timer>
#include <cc/ui/Transition>
#include <cc/ui/Label>

using namespace cc;
using namespace cc::ui;

class TestView: public View
{
    friend class Object;

    TestView()
    {
        size = Size{640, 480};
        color = Color{"#FFFFFF"};

        auto getClockText = []{
            Ref<const Date> date = Date::breakdown(System::now());
            return dec(date->hour(), 2) + "∶" + dec(date->minutes(), 2) + "∶" + dec(date->seconds(), 2);
        };

        Label *label = Label::create(this, getClockText(), TextStyle::create(Font::select("Sans", 48)));
        label->color = Color(0x00, 0x00, 0xFF, 0x20);
        label->pos->bind([=]{ return center() - label->size() / 2; });

        ease(label->angle, 0.5, easing::Bezier(0.5, -0.4, 0.5, 1.4));

        Timer::start(1, [=]{ label->text = getClockText(); label->angle += 45; });
    }
};

int main(int argc, char **argv)
{
    Application *app = Application::open(argc, argv);
    Window::open(Object::create<TestView>(), "Hello, world!");
    return app->run();
}
