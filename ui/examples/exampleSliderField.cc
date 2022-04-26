#include <cc/ColumnLayout>
#include <cc/Divider>
#include <cc/SliderField>
#include <cc/NumberCell>
#include <cc/Icon>

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
            SliderField{}
            .leading(Icon{Ideographic::VolumeLow})
            .trailing(Icon{Ideographic::VolumeHigh})
        )
        .add(
            Divider{}
        )
        .add(
            SliderField{}
            .leading(Icon{Ideographic::Thermometer})
            .trailing(NumberCell{})
        )
        .run();
}
