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

    Control box{200, 200};
    box->centerInParent();
    box->paper = Color{Material::Blue500};
    box->pointerClicked >>[=]{
        fout() << box->pointerPos() << nl;
        box->angle += 45;
    };
    Easing{box->angle, 0.5, Easing::Bezier{0.5, -0.4, 0.5, 1.4}};
    view << box;

    Window::open(view, "Click me!", WindowMode::Accelerated|WindowMode::VSync);

    return Application{}->run();
}
