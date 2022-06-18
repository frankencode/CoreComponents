#include <cc/ColumnLayout>
#include <cc/Divider>
#include <cc/Slider>
#include <cc/NumberCell>

int main()
{
    using namespace cc;

    return
        View{sp(400), sp(500)}
        .layout(
            ColumnLayout{}
            .margin(sp(16))
            .spacing(sp(8))
        )
        .add(
            Slider{}
            .leading(Icon::VolumeLow)
            .trailing(Icon::VolumeHigh)
        )
        .add(
            Divider{}
        )
        .add(
            Slider{}
            .leading(Icon::Thermometer)
            .trailing(NumberCell{})
            .value(50)
        )
        .run();
}
