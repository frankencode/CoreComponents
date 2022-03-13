#include <cc/ui/Application>
#include <cc/ui/Slider>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    Slider slider;

    Window{
        View{sp(640), sp(480)}
        .add(
            Slider{&slider}
            .centerInParent()
        )
    }.show();

    return Application{}.run();
}
