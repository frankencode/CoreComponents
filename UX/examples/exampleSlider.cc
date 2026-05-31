#include <cc/Divider>
#include <cc/Slider>
#include <cc/NumberCell>
#include <cc/layout>

int main()
{
    using namespace cc;

    return
        Column{sp(400), sp(500)}
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
