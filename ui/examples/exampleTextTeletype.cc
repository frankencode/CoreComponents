#include <cc/Random>
#include <cc/Array>
#include <cc/ui/Application>
#include <cc/ui/View>
#include <cc/ui/FontManager>

using namespace cc;
using namespace cc::ui;

class MainView: public View
{
    friend class Object;

    MainView()
    {
        size = Size{640, 480};
        paper = 0xFFFFFF;

        glyphRun_ =
            GlyphRun::typeset(
                "Es war einmal eine Königstochter, die ging hinaus in den Wald und setzte "
                "sich an einen kühlen Brunnen. Sie hatte eine goldene Kugel, die war ihr "
                "liebstes Spielwerk, die warf sie in die Höhe und fing sie wieder in der "
                "Luft und hatte ihre Lust daran. Einmal war die Kugel gar hoch geflogen, "
                "sie hatte die Hand schon ausgestreckt und die Finger gekrümmt, um sie "
                "wieder zufangen, da schlug sie neben vorbei auf die Erde, rollte und rollte "
                "und geradezu in das Wasser hinein.",
                Font(fontSize_) << Pitch::Fixed
            );

        Ref<Random> random = Random::open(0);
        fgColors_ = Array<Color>::create(glyphRun_->text()->count());
        bgColors_ = Array<Color>::create(glyphRun_->text()->count());
        Color fg, bg;
        for (int i = 0; i < glyphRun_->text()->count(); ++i) {
            if (!fg->isValid()) {
                int angle = random->get(0, 360);
                fg = Color::fromHue(angle);
                bg = Color::fromHue((angle + 180) % 360);
            }
            fgColors_->at(i) = fg;
            bgColors_->at(i) = bg;
            if (glyphRun_->text()->at(i) == ' ') fg = Color{};
        }

        auto updateTextWrap = [=]{
            wrappedGlyphRun_ = glyphRun_->wrap(size()[0] - 2 * margins_);
        };

        updateTextWrap();

        size->connect(updateTextWrap);
    }

    void paint() override
    {
        Painter p(this);
        p->setSource(0xE0F0FF);
        p->rectangle(Point{0, 0}, Size{margins_, size()[1]});
        p->rectangle(Point{size()[0]-margins_, 0}, Size{margins_, size()[1]});
        p->fill();
        p->setSource(0x000000);
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
    }

    const double margins_ = 30;
    const double fontSize_ = 20;
    Ref< Array<Color> > fgColors_;
    Ref< Array<Color> > bgColors_;
    Ref<const GlyphRun> glyphRun_;
    Ref<const GlyphRun> wrappedGlyphRun_;
};

int main(int argc, char **argv)
{
    Application *app = Application::open(argc, argv);
    Window::open<MainView>("Hello, world!");
    return app->run();
}
