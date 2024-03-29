#include <cc/OutlineButton>
#include <cc/debugging>

int main()
{
    using namespace cc;

    OutlineButton button;

    return
        View{sp(640), sp(480)}
        .add(
            OutlineButton{"12", Icon::Plus}
            .associate(&button)
            .onClicked([=]{
                CC_DEBUG;
            })
            .centerInParent()
        )
        .run();
}
