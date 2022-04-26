#include <cc/Application>
#include <cc/ColumnLayout>
#include <cc/Label>
#include <cc/Control>

int main()
{
    using namespace cc;

    Window{
        View{gu(80), gu(80)}
        .paper(Color::White)
        .add(
            View{}
            .setup([](auto &self){
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
                    Control c;
                    self.add(
                        Control{&c}
                        .cursor(shape)
                        .paper([c]{ return c.hover() ? "DeepSkyBlue" : "LightSkyBlue"; })
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
                .spacing(dp(2))
            )
            .centerInParent()
        )
    }.show();

    return Application{}.run();
}
