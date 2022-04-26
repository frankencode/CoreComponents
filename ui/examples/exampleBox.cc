#include <cc/Application>
#include <cc/DragArea>
#include <cc/Box>

int main()
{
    using namespace cc;

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
