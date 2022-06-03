#include <cc/SliderControl>
#include <cc/DEBUG>

int main()
{
    using namespace cc;

    SliderControl slider;

    return
        View{sp(640), sp(480)}
        .add(
            SliderControl{}
            .associate(&slider)
            .onUserInput([slider]{
                CC_INSPECT(slider.value());
            })
            .centerInParent()
        )
        .run();
}
