#include <cc/ui/Application>
#include <cc/ui/DragArea>
#include <cc/ui/Box>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    Window{
        DragArea{gu(80), gu(60)}
        .add(
            Box{gu(20), gu(20)}
            .radius(gu(2))
            .color("darkcyan")
            .centerInParent()
        )
    }.show();

    return Application{}.run();
}
