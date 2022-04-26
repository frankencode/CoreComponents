#include <cc/Application>
#include <cc/ElevatedButton>

int main()
{
    using namespace cc;

    Window{
        View{sp(640), sp(480)}
        .add(
            ElevatedButton{}
            .icon(Ideographic::Face)
            .text("Press me!")
            .centerInParent()
        )
    }.show();

    return Application{}.run();
}
