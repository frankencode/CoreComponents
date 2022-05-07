#include <cc/SwitchControl>

int main()
{
    using namespace cc;

    return
        View{sp(500), sp(500)}
        .add(
            SwitchControl{}
            .centerInParent()
        )
        .run();
}
