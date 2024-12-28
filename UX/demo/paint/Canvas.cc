#include <cc/paint/Canvas>
#include <cc/debugging>

namespace cc::paint {

struct Canvas::State final: public View::State
{
    State(double width, double height)
    {
        size(Size{width, height});
        paper(Color::White);

        onMousePressed(
            [this](const MouseEvent &event){
                mousePressed_ = true;
                prevMousePos_ = event.pos();
                return true;
            }
        );

        onMouseReleased(
            [this](const MouseEvent &event){
                mousePressed_ = false;
                return true;
            }
        );

        onMouseMoved(
            [this](const MouseEvent &event){
                if (mousePressed_) {
                    Painter{this}
                    .setPen(
                        Pen{"darkblue", sp(5)}
                        .lineCap(LineCap::Round)
                    )
                    .moveTo(prevMousePos_)
                    .lineTo(event.pos())
                    .stroke();
                    update();
                    prevMousePos_ = event.pos();
                }
                return mousePressed_;
            }
        );

        onFingerPressed(
            [this](const FingerEvent &event){
                prevFingerPos_ = event.pos();
                return true;
            }
        );

        onFingerMoved(
            [this](const FingerEvent &event){
                Painter{this}
                .setPen(
                    Pen{"darkred", sp(5)}
                    .lineCap(LineCap::Round)
                )
                .moveTo(prevFingerPos_)
                .lineTo(event.pos())
                .stroke();
                update();
                prevFingerPos_ = event.pos();
                CC_INSPECT(event.pressure());
                return true;
            }
        );
    }

    bool isStatic() const override { return false; }

    Size minSize() const override { return size(); }
    Size maxSize() const override { return size(); }

    void polish() override
    {
        if (firstTime_) {
            clear();
            firstTime_ = false;
        }
    }

    bool firstTime_ { true };
    bool mousePressed_ { false };
    Point prevMousePos_;
    Point prevFingerPos_;
};

Canvas::Canvas(double width, double height):
    View{new State{width, height}}
{}

} // namespace cc::paint
