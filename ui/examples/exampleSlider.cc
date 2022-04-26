#include <cc/Slider>

int main()
{
    using namespace cc;

    return
        View{sp(640), sp(480)}
        .add(
            Slider{}
            .centerInParent()
        )
        .run();
}
