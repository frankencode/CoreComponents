#include <cc/Application>
#include <cc/Slider>

int main()
{
    using namespace cc;

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
