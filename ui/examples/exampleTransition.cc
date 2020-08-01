#include <cc/ui/Application>
#include <cc/ui/Easing>
#include <cc/ui/colors>
#include <cc/stdio>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    View view{640, 480};
    view->paper = Material::White;

    Control blueBox{200, 200};
    blueBox->centerInParent();
    blueBox->paper = Color{Material::Blue500};
    blueBox->pressed >>[=]{
        if (!blueBox->pressed()) fout() << blueBox->pointerPos() << nl;
        blueBox->angle += 45;
    };
    Easing{blueBox->angle, 0.5, Easing::Bezier{0.5, -0.4, 0.5, 1.4}};
    view << blueBox;

    Window::open(view, "Click me!", WindowMode::Accelerated|WindowMode::VSync);

    return Application{}->run();
}
