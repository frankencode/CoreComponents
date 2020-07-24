#include <cc/ui/Application>
#include <cc/ui/FontManager>
#include <cc/Array>
#include <cc/Random>
#include <cc/DEBUG>

using namespace cc;
using namespace cc::ui;

class MainView: public View
{
    friend class Object;

    class Instance: public View::Instance
    {
        friend class MainView;

        Instance(double width, double height)
        {
            size = Size{width, height};
            paper = Color::White;

            glyphRun_ =
                GlyphRun::typeset(
                    "Es war einmal eine Königstochter, die ging hinaus in den Wald und setzte "
                    "sich an einen kühlen Brunnen. Sie hatte eine goldene Kugel, die war ihr "
                    "liebstes Spielwerk, die warf sie in die Höhe und fing sie wieder in der "
                    "Luft und hatte ihre Lust daran. Einmal war die Kugel gar hoch geflogen, "
                    "sie hatte die Hand schon ausgestreckt und die Finger gekrümmt, um sie "
                    "wieder zufangen, da schlug sie neben vorbei auf die Erde, rollte und rollte "
                    "und geradezu in das Wasser hinein.",
                    Font{fontSize_} << Pitch::Fixed
                );

            Random random{0};
            fgColors_ = Array<Color>{glyphRun_->text()->count()};
            bgColors_ = Array<Color>{glyphRun_->text()->count()};
            Color fg, bg;
            for (int i = 0; i < glyphRun_->text()->count(); ++i) {
                if (!fg->isValid()) {
                    int angle = random->get(0, 360);
                    fg = Color::fromHue(angle);
                    bg = Color::fromHue((angle + 180) % 360);
                }
                fgColors_[i] = fg;
                bgColors_[i] = bg;
                if (glyphRun_->text()->at(i) == ' ') fg = Color{};
            }

            auto updateTextWrap = [=]{
                wrappedGlyphRun_ = glyphRun_->wrap(size()[0] - 2 * margins_);
            };

            updateTextWrap();

            size >>[=]{ updateTextWrap(); };

            paint >>[=]{
                Painter p{this};
                p->setSource(Color{0xE0F0FF});
                p->rectangle(Point{0, 0}, Size{margins_, size()[1]});
                p->rectangle(Point{size()[0]-margins_, 0}, Size{margins_, size()[1]});
                p->fill();
                p->setSource(Color::Black);
                p->showGlyphRun(
                    Point{margins_, margins_ + fontSize_}, wrappedGlyphRun_,
                    [=](int i) {
                        if (fgColors_->has(i)) return fgColors_->at(i);
                        return Color{};
                    },
                    [=](int i) {
                        if (bgColors_->has(i)) return bgColors_->at(i);
                        return Color{};
                    }
                );
            };
        }

        const double margins_ = 30;
        const double fontSize_ = 20;
        Array<Color> fgColors_;
        Array<Color> bgColors_;
        Ref<const GlyphRun> glyphRun_;
        Ref<const GlyphRun> wrappedGlyphRun_;
    };

public:
    MainView(double width, double height):
        View{new Instance{width, height}}
    {}
};

int main(int argc, char **argv)
{
    Window::open(MainView{640, 480}, "Hello, world!", WindowMode::Accelerated|WindowMode::VSync);
    return Application{}->run();
}
