#include <cc/ui/Application>
#include <cc/ui/ManagedRowLayout>
#include <cc/ui/Label>
#include <cc/ui/Slider>
#include <cc/DEBUG>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    View view;

    Window{
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
    }.show();

    return Application{}.run();
}
