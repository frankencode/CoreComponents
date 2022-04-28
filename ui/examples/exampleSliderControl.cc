#include <cc/SliderControl>

int main()
{
    using namespace cc;

    return
        View{sp(640), sp(480)}
        .add(
            SliderControl{}
            .centerInParent()
        )
        .run();
}
