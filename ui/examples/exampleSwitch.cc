#include <cc/SwitchControl>

int main()
{
    using namespace cc;

    return
        View{sp(640), sp(480)}
        .add(
            SwitchControl{}
            .centerInParent()
        )
        .run();
}
