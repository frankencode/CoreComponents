#include <cc/ElevatedButton>
#include <cc/debugging>

int main()
{
    using namespace cc;

    ElevatedButton button;

    return
        View{sp(640), sp(480)}
        .add(
            ElevatedButton{"USER", Icon::Plus}
            .associate(&button)
            .onClicked([=]{
                CC_DEBUG;
            })
            .centerInParent()
        )
        .run();
}
