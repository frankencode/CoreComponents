#include <cc/DragArea>
#include <cc/Box>

int main()
{
    using namespace cc;

    return
        DragArea{gu(80), gu(60)}
        .add(
            Box{gu(20), gu(20)}
            .radius(gu(2))
            .color("yellow")
            .border(Pen{"darkgray", sp(2)})
            .centerInParent()
        )
        .run();
}
