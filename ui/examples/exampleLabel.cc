#include <cc/debug>
#include <cc/System>
#include <cc/Date>
#include <cc/ui/Application>
#include <cc/ui/Timer>
#include <cc/ui/Transition>
#include <cc/ui/Label>

using namespace cc;
using namespace cc::ui;

class MainView: public View
{
    friend class Object;

    MainView()
    {
        size = Size{640, 480};
        inheritPaper();

        auto getClockText = []{
            Ref<const Date> date = Date::breakdown(System::now());
            return dec(date->hour(), 2) + "∶" + dec(date->minutes(), 2) + "∶" + dec(date->seconds(), 2);
        };

        Label *label = add<Label>(getClockText());
        label->font = Font { sp(40) };
        label->paper = 0xD0D0FF;
        label->ink = 0x000000;
        label->margin = sp(20);
        label->pos->bind([=]{ return center() - label->center(); });

        easeOn(label->angle, 0.5, easing::Bezier(0.5, -0.4, 0.5, 1.4));
        Timer::start(1, [=]{ label->text = getClockText(); label->angle += 45; });
    }

    bool onKeyPressed(const KeyEvent *event)
    {
        if (+(event->modifiers() & KeyModifier::Control))
        {
            if (event->keyCode() == '+')
                Application::instance()->textZoom += 0.1;
            else if (event->keyCode() == '-')
                Application::instance()->textZoom -= 0.1;
        }

        return true;
    }
};

int main(int argc, char **argv)
{
    Application *app = Application::open(argc, argv);
    Window::open<MainView>("Hello, world!");
    return app->run();
}
