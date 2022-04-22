#include <cc/ui/Application>
#include <cc/ui/ColumnLayout>
#include <cc/ui/RowLayout>
#include <cc/ui/Label>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    double gap = Font::defaultSize() * 1.5;

    View view;

    Window{
        View{&view}
        .centerInParent()
        .paper(0xD0D0FF)
        .add(Label{"• Item 1"})
        .add(Label{"• Item 2"})
        .add(Label{"• Item 3"})
        .add(
            View{}
            .paper(0xD0FFD0)
            .add(Label{"◦ Item A B C"})
            .add(Label{"◦ Item B D E"})
            .add(Label{"◦ Item C D F"})
            .layout(
                RowLayout{}
                .spacing(gap)
                .margin(gap)
            )
        )
        .layout(
            ColumnLayout{}
            .spacing(gap)
            .margin(gap)
        )
    }.show();

    return Application{}.run();
}
