#include <cc/ManagedRowLayout>
#include <cc/Slider>
#include <cc/Label>
#include <cc/DEBUG>

int main()
{
    using namespace cc;

    View view;

    return
        View{&view}
        .size(sp(480), sp(48))
        .layout(
            ManagedRowLayout{}
            .align(RowAlign::Center)
        )
        .add(Label{"Temperature"})
        .add(Slider{})
        .add(Label{"Degree Celsius"})
        .attach([=]{
            CC_INSPECT(view.minSize());
            CC_INSPECT(view.maxSize());
        })
        .run();
}
