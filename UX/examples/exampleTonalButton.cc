#include <cc/TonalButton>
#include <cc/debugging>

int main()
{
    using namespace cc;

    TonalButton button;

    return
        View{sp(640), sp(480)}
        .add(
            TonalButton{"User", Icon::Plus}
            .associate(&button)
            .onClicked([=]{
                CC_DEBUG;
            })
            .centerInParent()
        )
        .run();
}
