#include <cc/ui/Application>
#include <cc/ui/Easing>
#include <cc/ui/Label>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    View view{640, 480};

    Label label{"Press + | - (zoom in/out)"};
    label->font = Font{40};
    label->ink = Color::Black;
    label->keyPressed >>[=](const KeyEvent *event){
        if (event->keyCode() == '+')
            label->font *= 2;
        else if (event->keyCode() == '-')
            label->font /= 2;
        return true;
    };
    Easing{label->font, 1, Easing::Bezier{0.5, -0.4, 0.5, 1.4}};
    view << label;

    Window::open(view, "Hello, world!", WindowMode::Accelerated|WindowMode::VSync);

    return Application{}->run();
}
