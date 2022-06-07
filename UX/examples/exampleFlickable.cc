#include <cc/Flickable>
#include <cc/FlickableIndicator>
#include <cc/ColumnLayout>
#include <cc/Label>

int main()
{
    using namespace cc;

    const double gap = Font::defaultSize();

    return
        Flickable{
            640, 480,
            Pane{}
            .layout(
                ColumnLayout{}
                .spacing(gap)
                .margin(gap)
            )
            .populate([](auto target) {
                for (int i = 0; i < 100; ++i) {
                    target.add(Label{"Item " + str(i + 1)});
                }
            })
        }
        .add(FlickableIndicator{})
        .run();
}
