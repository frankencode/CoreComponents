#include <cc/FloatingButton>
#include <cc/debugging>

int main()
{
    using namespace cc;

    return
        View{sp(640), sp(480)}
        .add(
            FloatingButton{"USER", Icon::Plus}
            .onClicked([=]{
                CC_DEBUG;
            })
            .centerInParent()
        )
        .run();
}
