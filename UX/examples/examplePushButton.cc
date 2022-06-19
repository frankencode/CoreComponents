#include <cc/PushButton>
#include <cc/Format>
#include <cc/DEBUG>

int main()
{
    using namespace cc;

    PushButton button;

    return
        View{sp(640), sp(480)}
        .add(
            PushButton{"USER", Icon::Plus}
            .associate(&button)
            .onClicked([=]{
                CC_DEBUG;
            })
            .centerInParent()
        )
        .run();
}
