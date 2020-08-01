#include <cc/ui/Application>
#include <cc/ui/SlideView>
#include <cc/ui/ColumnLayout>
#include <cc/ui/Easing>
#include <cc/ui/Label>
#include <cc/DEBUG>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    SlideView view{640, 480};
    view->paper = Color::White;
    // Easing(view->slideCarrier()->pos, 0.5, Easing::Bezier(0.5, -0.4, 0.5, 1.4));

    View slide1{640, 480};
    {
        Label label{"Press (->)"};
        label->paper = Color::Red;
        label->centerInParent();
        slide1 << label;
    }
    view << slide1;

    #if 0
    View slide2;
    {
        View box;
        box->paper = 0xD0D0FF;
        box->centerInParent();

        ColumnLayout{box};
        box << Label{"• Item 1"};
        box << Label{"• Item 2"};
        box << Label{"• Item 3"};

        slide2 << box;
    }
    view << slide2;

    view->keyPressed >>[=](const KeyEvent *event)
    {
        if (event->scanCode() == ScanCode::Key_Left)
        {
            view->currentIndex -= 1;
        }
        else if (event->scanCode() == ScanCode::Key_Right)
        {
            view->currentIndex += 1;
        }
        else if ('0' <= +event->keyCode() && +event->keyCode() <= '9')
        {
            view->currentIndex = +event->keyCode() - '1' + 10 * (+event->keyCode() == '0');
        }

        return true;
    };
    #endif

    Window::open(view, "Hello, world!", WindowMode::Accelerated|WindowMode::VSync);

    return Application{}->run();
}
