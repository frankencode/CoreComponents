#include <cc/ui/Application>
#include <cc/ui/ScrollBar>
#include <cc/ui/Label>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    Window{
        Control{gu(80), gu(60)}
        .add(
            ScrollBar{}
            // .orientation(Orientation::Horizontal)
            .visibleOffset(-gu(30))
            .totalExtent(gu(120))
            .autoHide(true)
        )
    }.show();

    return Application{}.run();
}
