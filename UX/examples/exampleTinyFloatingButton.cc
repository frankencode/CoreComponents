#include <cc/TinyFloatingButton>
#include <cc/Format>
#include <cc/DEBUG>

int main()
{
    using namespace cc;

    TinyFloatingButton button;

    return
        View{sp(640), sp(480)}
        .add(
            TinyFloatingButton{Icon::Plus}
            .associate(&button)
            .onClicked([=]{
                CC_DEBUG;
            })
            .centerInParent()
        )
        .run();
}
