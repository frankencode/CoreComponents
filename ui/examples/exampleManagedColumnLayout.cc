#include <cc/ui/Application>
#include <cc/ui/ManagedColumnLayout>
#include <cc/ui/Text>
#include <cc/ui/Label>
#include <cc/DEBUG>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    double gap = Font::defaultSize() * 1.5;

    View view;

    Window{
        View{&view}
        .paper(0xD0D0FF)
        .layout(
            ManagedColumnLayout{}
            .spacing(gap)
            .margin(gap)
        )
        .add(Text{"• Item 1"})
        .add(Text{"• Item 2"})
        .add(Text{"• Item 3"})
        .add(
            View{}
            .paper(0xD0FFD0)
            .layout(
                ManagedColumnLayout{}
                .indent(gap)
                .spacing(gap)
                .margin(gap)
            )
            .add(Text{"◦ Item A B C"})
            .add(Text{"◦ Item B C D"})
            .add(Text{"◦ Item C D E"})
        )
    }.show();

    CC_INSPECT(view.minSize());
    CC_INSPECT(view.maxSize());

    return Application{}.run();
}
