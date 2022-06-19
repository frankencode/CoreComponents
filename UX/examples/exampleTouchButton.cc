#include <cc/TouchButton>
#include <cc/Format>
#include <cc/DEBUG>

int main()
{
    using namespace cc;

    TouchButton button;

    return
        View{sp(640), sp(480)}
        .add(
            TouchButton{"User", Icon::Plus}
            .associate(&button)
            .onClicked([=]{
                CC_DEBUG;
            })
            .centerInParent()
        )
        .run();
}
