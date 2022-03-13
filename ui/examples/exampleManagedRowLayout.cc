#include <cc/ui/Application>
#include <cc/ui/ManagedColumnLayout>
#include <cc/ui/ManagedRowLayout>
#include <cc/ui/Text>
#include <cc/ui/Label>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    double gap = Font::defaultSize() * 1.5;

    Window{
        View{}
        .centerInParent()
        .paper(0xD0D0FF)
        .add(Text{"• Item 1"})
        .add(Text{"• Item 2"})
        .add(Text{"• Item 3"})
        .add(
            View{}
            .paper(0xD0FFD0)
            .add(Text{"◦ Item A B C"})
            .add(Text{"◦ Item B D E"})
            .add(Text{"◦ Item C D F"})
            .layout(
                ManagedRowLayout{}
                .spacing(gap)
                .margin(gap)
            )
        )
        .layout(
            ManagedColumnLayout{}
            .spacing(gap)
            .margin(gap)
        )
    }.show();

    return Application{}.run();
}
