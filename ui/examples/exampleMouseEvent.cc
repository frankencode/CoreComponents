#include <cc/ui/Application>
#include <cc/System>
#include <cc/stdio>

using namespace cc;
using namespace cc::ui;

class MainView: public Control
{
public:
    class Instance: public Control::Instance
    {
        friend class MainView;

        Instance()
        {
            size = Size{640, 480};
            paper = Color::White;
            paint >>[=]{ onPaint(); };
        }

        void onPaint()
        {
            Painter p{this};

            p->translate(size()/2);

            p->setSource(Color::Red(50));
            p->circle(-Point{30, 0}, 60);
            p->fill();

            p->setSource(Color::Green(50));
            p->circle(Point{0, 0}, 60);
            p->fill();

            p->setSource(Color::Blue(50));
            p->circle(Point{30, 0}, 60);
            p->fill();
        }

        bool onPointerPressed(const PointerEvent *event) override
        {
            fout() << "Pressed at " << event->pos() << nl;
            return true;
        }

        bool onPointerReleased(const PointerEvent *event) override
        {
            fout() << "Released at " << event->pos() << nl;
            return true;
        }

        bool onPointerClicked(const PointerEvent *event) override
        {
            fout() << "Clicked at " << event->pos() << nl;
            return true;
        }

        bool onPointerMoved(const PointerEvent *event) override
        {
            fout("Moved to %% (%%, %%)\n") << event->pos() << event->time() << System::now();
            return true;
        }
    };

    MainView():
        Control{new Instance}
    {}
};

int main()
{
    Window::open(MainView{}, "Hello, world!", WindowMode::Accelerated|WindowMode::VSync);
    return Application{}->run();
}
