#include <cc/Label>
#include <cc/Control>
#include <cc/layout>

int main()
{
    using namespace cc;

    return
        Column{}
        .paper(Color::White)
        .populate([](auto target){
            const double gap = style().defaultFont().size();
            for (auto shape: List<CursorShape>{
                CursorShape::Arrow,
                CursorShape::IBeam,
                CursorShape::Wait,
                CursorShape::CrossHair,
                CursorShape::WaitArrow,
                CursorShape::ResizeNorthWestSouthEast,
                CursorShape::ResizeNorthEastSouthWest,
                CursorShape::ResizeWestEast,
                CursorShape::ResizeNorthSouth,
                CursorShape::ResizeCross,
                CursorShape::Forbidden,
                CursorShape::Hand
            }) {
                Control control;
                target.add(
                    Control{&control}
                    .cursor(shape)
                    .paper([control]{ return control.hover() ? "DeepSkyBlue" : "LightSkyBlue"; })
                    .layout(
                        ColumnLayout{}
                        .margin(gap)
                    )
                    (Label{str(shape)})
                );
            }
        })
        .run();
}
