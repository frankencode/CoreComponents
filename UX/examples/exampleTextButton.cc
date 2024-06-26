#include <cc/TextButton>
#include <cc/Format>
#include <cc/debugging>

int main()
{
    using namespace cc;

    TextButton button;

    return
        View{sp(640), sp(480)}
        .add(
            TextButton{"User", Icon::Plus}
            .associate(&button)
            .onClicked([=]{
                CC_DEBUG;
            })
            .centerInParent()
        )
        .run();
}
