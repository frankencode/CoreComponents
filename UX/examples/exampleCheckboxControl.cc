#include <cc/CheckboxControl>

int main()
{
    using namespace cc;

    return
        View{sp(640), sp(480)}
        .add(
            CheckboxControl{}
            // .focus(true)
            .centerInParent()
        )
        .run();
}
