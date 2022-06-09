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
            .leading(Ideographic::VolumeLow)
            .trailing(Ideographic::VolumeHigh)
        )
        .add(
            Divider{}
        )
        .add(
            Slider{}
            .leading(Ideographic::Thermometer)
            .trailing(NumberCell{})
            .value(50)
        )
        .run();
}
