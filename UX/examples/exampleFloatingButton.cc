#include <cc/FloatingButton>
#include <cc/debugging>

int main()
{
    using namespace cc;

    FloatingButton button;

    return
        View{sp(640), sp(480)}
        .add(
            FloatingButton{"USER", Icon::Plus}
            .associate(&button)
            .onClicked([=]{
                CC_DEBUG;
            })
            .centerInParent()
        )
        .run();
}
