#include <cc/ColumnLayout>
#include <cc/Label>

int main()
{
    using namespace cc;

    double gap = Font::defaultSize() * 1.5;

    return
        View{}
        .paper(0xD0D0FF)
        .add(Label{"• Item 1"})
        .add(Label{"• Item 2"})
        .add(Label{"• Item 3"})
        .add(
            View{}
            .paper(0xD0FFD0)
            .add(Label{"◦ Item A B C"})
            .add(Label{"◦ Item B C D"})
            .add(Label{"◦ Item C D E"})
            .layout(
                ColumnLayout{}
                .indent(gap)
                .spacing(gap)
                .margin(gap)
            )
        )
        .layout(
            ColumnLayout{}
            .spacing(gap)
            .margin(gap)
        )
        .run();
}
