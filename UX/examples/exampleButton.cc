#include <cc/Button>
#include <cc/Format>
#include <cc/DEBUG>

int main()
{
    using namespace cc;

    Button button;

    return
        View{sp(640), sp(480)}
        .add(
            Button{"USER", Icon::Plus}
            .associate(&button)
            .onClicked([=]{
                CC_DEBUG;
            })
            .centerInParent()
        )
        .run();
}
