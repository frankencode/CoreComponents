#include <cc/ui/Application>
#include <cc/ui/ScrollBar>
#include <cc/ui/Label>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    Control control{gu(80), gu(60)};

    ScrollBar scroll;
    // scroll.orientation(Orientation::Horizontal);
    scroll.visibleOffset(-gu(30));
    scroll.totalExtent(gu(120));
    scroll.autoHide(true);
    control.add(scroll);

    Window{control}.show();

    return Application{}.run();
}
