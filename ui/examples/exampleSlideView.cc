#include <cc/ui/Application>
#include <cc/ui/SlideView>
#include <cc/ui/ColumnLayout>
#include <cc/ui/Label>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    SlideView view{640, 480};

    View slide1;
    {
        Label label{"Press (->)"};
        label->centerInParent();
        slide1 << label;
    }
    view << slide1;

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

    View slide3;
    {
        View box;
        box->paper = 0xD0FFD0;
        box->centerInParent();

        ColumnLayout{box};
        box << Label{"• Item A"};
        box << Label{"• Item B"};
        box << Label{"• Item C"};

        slide3 << box;
    }
    view << slide3;

    Window::open(view, "Hello, world!", WindowMode::Accelerated|WindowMode::VSync);

    return Application{}->run();
}
