#include <cc/OutlineButton>
#include <cc/debugging>

int main()
{
    using namespace cc;

    return
        View{sp(640), sp(480)}
        .add(
            OutlineButton{"12", Icon::Plus}
            .onClicked([=]{
                CC_DEBUG;
            })
            .centerInParent()
        )
        .run();
}
