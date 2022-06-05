#include <cc/FloatingActionButton>
#include <cc/Format>
#include <cc/DEBUG>

int main()
{
    using namespace cc;

    FloatingActionButton button;

    return
        View{sp(640), sp(480)}
        .add(
            FloatingActionButton{"USER"}
            // FloatingActionButton{}
            .associate(&button)
            // .icon(Ideographic::Reload)
            .icon(Ideographic::Plus)
            .onClicked([=]{
                CC_DEBUG;
            })
            .centerInParent()
        )
        .run();
}
