#include <cc/paint/Canvas>
#include <cc/debugging>

namespace cc::paint {

struct Canvas::State final: public View::State
{
    State(double width, double height)
    {
        size(Size{width, height});
        paper(Color::White);

        redPen_ = Pen{"darkred", sp(5)}.lineCap(LineCap::Round);
        greenPen_ = Pen{"darkgreen", sp(5)}.lineCap(LineCap::Round);
        bluePen_ = Pen{"darkblue", sp(5)}.lineCap(LineCap::Round);
        erasePen_ = Pen{"white", sp(20)}.lineCap(LineCap::Round);

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
                    .setPen(greenPen_)
                    .moveTo(prevMousePos_)
                    .lineTo(event.pos())
                    .stroke();
                    update();
                    prevMousePos_ = event.pos();
                }
                return mousePressed_;
            }
        );

        onPenMoved(
            [this](const PenEvent &event){
                if (penDown_) {
                    Painter{this}
                    .setPen((event.buttons() & PenButton::EraseTip) ? erasePen_ : bluePen_)
                    .moveTo(prevPenPos_)
                    .lineTo(event.pos())
                    .stroke();
                    update();
                }
                else {
                    penDown_ = true;
                }
                prevPenPos_ = event.pos();
                return true;
            }
        );

        onPenHovered(
            [this](const PenEvent &event){
                penDown_ = false;
                return true;
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
                .setPen(redPen_)
                .moveTo(prevFingerPos_)
                .lineTo(event.pos())
                .stroke();
                update();
                prevFingerPos_ = event.pos();
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
    bool penDown_ { false };
    Point prevMousePos_;
    Point prevFingerPos_;
    Point prevPenPos_;
    Pen redPen_;
    Pen greenPen_;
    Pen bluePen_;
    Pen erasePen_;
};

Canvas::Canvas(double width, double height):
    View{new State{width, height}}
{}

} // namespace cc::paint
