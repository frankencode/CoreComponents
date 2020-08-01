#include <cc/ui/Application>
#include <cc/System>
#include <cc/stdio>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    Control control{640, 480};

    control->paint <<[=]{
        Painter p{control};

        p->translate(control->size()/2);

        p->setSource(Color::Red(50));
        p->circle(-Point{30, 0}, 60);
        p->fill();

        p->setSource(Color::Green(50));
        p->circle(Point{0, 0}, 60);
        p->fill();

        p->setSource(Color::Blue(50));
        p->circle(Point{30, 0}, 60);
        p->fill();
    };

    control->pointerPressed >>[=](const PointerEvent *event)
    {
        fout() << "Pressed at " << event->pos() << nl;
        return true;
    };

    control->pointerReleased >>[=](const PointerEvent *event)
    {
        fout() << "Released at " << event->pos() << nl;
        return true;
    };

    control->pointerClicked >>[=](const PointerEvent *event)
    {
        fout() << "Clicked at " << event->pos() << nl;
        return true;
    };

    control->pointerMoved >>[=](const PointerEvent *event)
    {
        fout("Moved to %% (%%, %%)\n") << event->pos() << event->time() << System::now();
        return true;
    };

    Window::open(control, "Hello, world!", WindowMode::Accelerated|WindowMode::VSync);

    return Application{}->run();
}
