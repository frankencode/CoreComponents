#include <cc/Application>
#include <cc/ScrollBar>
#include <cc/Label>

int main()
{
    using namespace cc;

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
