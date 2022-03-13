#include <cc/ui/Application>
#include <cc/ui/ColumnLayout>
#include <cc/ui/Label>
#include <cc/ui/Control>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    struct CursorControl: public Control
    {
        CursorControl(CursorShape shape)
        {
            auto self = me();
            paper([=]{ return self.hover() ? "DeepSkyBlue" : "LightSkyBlue"; });
            cursor(shape);
            layout(
                ColumnLayout{}
                .margin(style().defaultFont().size())
            );
            add(Label{str(shape)});
        }
    };

    Window{
        View{640, 480}
        .paper(Color::White)
        .add(
            View{}
            .layout(
                ColumnLayout{}
                .spacing(dp(2))
            )
            .centerInParent()
            .add(CursorControl{CursorShape::Arrow})
            .add(CursorControl{CursorShape::IBeam})
            .add(CursorControl{CursorShape::Wait})
            .add(CursorControl{CursorShape::CrossHair})
            .add(CursorControl{CursorShape::WaitArrow})
            .add(CursorControl{CursorShape::ResizeNorthWestSouthEast})
            .add(CursorControl{CursorShape::ResizeNorthEastSouthWest})
            .add(CursorControl{CursorShape::ResizeWestEast})
            .add(CursorControl{CursorShape::ResizeNorthSouth})
            .add(CursorControl{CursorShape::ResizeCross})
            .add(CursorControl{CursorShape::Forbidden})
            .add(CursorControl{CursorShape::Hand})
        )
    }.show();

    return Application{}.run();
}
