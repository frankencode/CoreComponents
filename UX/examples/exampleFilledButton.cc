#include <cc/FilledButton>
#include <cc/debugging>

int main()
{
    using namespace cc;

    FilledButton button;

    return
        View{sp(640), sp(480)}
        .add(
            FilledButton{"User", Icon::Plus}
            .associate(&button)
            .onClicked([=]{
                CC_DEBUG;
            })
            .centerInParent()
        )
        .run();
}
