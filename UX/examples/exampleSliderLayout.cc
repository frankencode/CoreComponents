#include <cc/SliderControl>
#include <cc/Label>
#include <cc/layout>
#include <cc/debugging>

int main()
{
    using namespace cc;

    View view;

    return
        View{{sp(480), sp(48)}, &view}
        .layout(
            ManagedRowLayout{}
            .align(RowAlign::Center)
        )
        .add(Label{"Temperature"})
        .add(SliderControl{})
        .add(Label{"Degree Celsius"})
        .attach([=]{
            CC_INSPECT(view.minSize());
            CC_INSPECT(view.maxSize());
        })
        .run();
}
