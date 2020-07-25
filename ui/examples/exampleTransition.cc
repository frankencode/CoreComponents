#include <cc/ui/Application>
#include <cc/ui/Easing>
#include <cc/ui/colors>
#include <cc/stdio>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    Control control{640, 480};
    control->paper = Material::White;
    control->paint >>[=]{
        Painter p{control};
        p->translate(control->center());
        p->setSource(Color{Material::Blue500});
        p->rectangle(-Point{50, 50}, Size{100, 100});
        p->fill();
    };
    control->pressed >>[=]{
        if (!control->pressed()) fout() << control->pointerPos() << nl;
        control->angle += 45;
    };
    Easing{control->angle, 0.5, Easing::Bezier{0.5, -0.4, 0.5, 1.4}};
    Window::open(control, "Click me!", WindowMode::Accelerated|WindowMode::VSync);
    return Application{}->run();
}
