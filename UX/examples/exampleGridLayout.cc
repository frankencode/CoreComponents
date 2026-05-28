#include <cc/GridLayout>
#include <cc/Label>

int main()
{
    using namespace cc;

    double gap = Font::defaultSize() * 1.5;

    return
        View{}
        .add(Label{"Device type:"}).add(Label{"ear cleaner"})
        .add(Label{"Serial number:"}).add(Label{"9785916495620"})
        .add(Label{"Date of manufacture:"}).add(Label{"20260529"})
        .layout(
            GridLayout{}
            .spacing(gap)
            .margin(gap)
        )
        .run();
}
