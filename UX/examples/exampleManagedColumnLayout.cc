#include <cc/ManagedColumnLayout>
#include <cc/Text>
#include <cc/debugging>

int main()
{
    using namespace cc;

    double gap = Font::defaultSize() * 1.5;

    View view;

    return
        View{}
        .associate(&view)
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
        .attach([=]{
            CC_INSPECT(view.minSize());
            CC_INSPECT(view.maxSize());
        })
        .run();
}
