#include <cc/debug>
#include <cc/System>
#include <cc/Date>
#include <cc/ui/Application>
#include <cc/ui/Timer>
#include <cc/ui/Transition>
#include <cc/ui/Label>

using namespace cc;
using namespace cc::ui;

class MainView: public View, public KeyInput
{
    friend class Object;

    MainView():
        KeyInput(this)
    {
        size = Size{640, 480};
        color = Color{"#FFFFFF"};

        auto getClockText = []{
            Ref<const Date> date = Date::breakdown(System::now());
            return dec(date->hour(), 2) + "∶" + dec(date->minutes(), 2) + "∶" + dec(date->seconds(), 2);
        };

        Application::instance()->textZoom = 28;

        Label *label = Label::create(this, getClockText());
        label->color = Color{"#D0D0FF"};
        label->pos->bind([=]{ return center() - label->size() / 2; });

        easeOn(label->angle, 0.5, easing::Bezier(0.5, -0.4, 0.5, 1.4));
        Timer::start(1, [=]{ label->text = getClockText(); label->angle += 45; });

        keyPressed->connect([=]{
            if (+(key()->modifiers() & KeyModifier::Control)) {
                if (key()->keyCode() == '+') {
                    Application::instance()->textZoom += 4;
                }
                else if (key()->keyCode() == '-') {
                    Application::instance()->textZoom -= 4;
                }
            }
        });
    }
};

int main(int argc, char **argv)
{
    Application *app = Application::open(argc, argv);
    Window::open(Object::create<MainView>(), "Hello, world!", WindowMode::Default|WindowMode::Accelerated);
    return app->run();
}
