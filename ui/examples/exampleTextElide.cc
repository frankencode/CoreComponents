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
                Font(fontSize_)
            );

        auto updateTextElide = [=]{
            elidedGlyphRun_ = glyphRun_->elide(size()[0] - 2 * margins_);
        };

        updateTextElide();

        size->connect(updateTextElide);
    }

    void paint() override
    {
        Painter p(this);
        p->setSource(0xE0F0FF);
        p->rectangle(Point{0, 0}, Size{margins_, size()[1]});
        p->rectangle(Point{size()[0]-margins_, 0}, Size{margins_, size()[1]});
        p->fill();
        p->setSource(0x000000);
        p->showGlyphRun(Point{margins_, margins_ + fontSize_}, elidedGlyphRun_);
    }

    const double margins_ = 30;
    const double fontSize_ = 20;
    Ref<const GlyphRun> glyphRun_;
    Ref<const GlyphRun> elidedGlyphRun_;
};

int main(int argc, char **argv)
{
    auto app = Application::open(argc, argv);
    Window::open<MainView>("Hello, world!");
    return app->run();
}
