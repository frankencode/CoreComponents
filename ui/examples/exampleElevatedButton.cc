#include <cc/ui/Application>
#include <cc/ui/ElevatedButton>

int main()
{
    using namespace cc;
    using namespace cc::ui;

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
