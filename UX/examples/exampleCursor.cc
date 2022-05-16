#include <cc/ColumnLayout>
#include <cc/Label>
#include <cc/Control>

int main()
{
    using namespace cc;

    return
        View{gu(80), gu(80)}
        .paper(Color::White)
        .add(
            View{}
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
                        Control{}
                        .associate(&control)
                        .cursor(shape)
                        .paper([control]{ return control.hover() ? "DeepSkyBlue" : "LightSkyBlue"; })
                        .layout(
                            ColumnLayout{}
                            .margin(gap)
                        )
                        .add(Label{str(shape)})
                    );
                }
            })
            .layout(
                ColumnLayout{}
                .spacing(sp(2))
            )
            .centerInParent()
        )
        .run();
}
