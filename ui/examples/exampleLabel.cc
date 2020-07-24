#include <cc/ui/Application>
#include <cc/ui/Label>
#include <cc/ui/Timer>
#include <cc/ui/Easing>
#include <cc/System>
#include <cc/Date>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    View view;
    view->size = Size{640, 480};
    view->paper = Color::White;

    Label label;
    label->paper = 0xD0D0FF;
    label->ink = Color::Black;
    label->margin = sp(20);
    label->font = sp(40);
    label->pos <<[=]{ return view->center() - label->center(); };

    Easing{label->angle, 0.5, easing::Bezier{0.5, -0.4, 0.5, 1.4}};
    Easing{label->size, 0.5, easing::Bezier{0.5, -0.4, 0.5, 1.4}};

    view << label;

    Timer timer{1};
    timer->timeout >>[=]{
        Date date{System::now()};
        label->text = dec(date->hour(), 2) + "∶" + dec(date->minutes(), 2) + "∶" + dec(date->seconds(), 2);
        label->angle += 45;
    };
    timer->start();

    Window::open(view, "Hello, world!", WindowMode::Accelerated|WindowMode::VSync);

    return Application{}->run();
}
