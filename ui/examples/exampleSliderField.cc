#include <cc/ui/Application>
#include <cc/ui/ColumnLayout>
#include <cc/ui/Divider>
#include <cc/ui/SliderField>
#include <cc/ui/NumberCell>
#include <cc/ui/Icon>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    Window{
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
    }.show();

    return Application{}.run();
}
