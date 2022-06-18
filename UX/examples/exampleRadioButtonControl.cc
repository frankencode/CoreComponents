#include <cc/RadioButtonControl>

int main()
{
    using namespace cc;

    return
        View{sp(640), sp(480)}
        .add(
            RadioButtonControl{}
            // .focus(true)
            .centerInParent()
        )
        .run();
}
