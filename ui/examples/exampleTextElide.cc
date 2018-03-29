#include <cc/ui/Application>
#include <cc/ui/FontManager>
#include <cc/ui/TypeSetter>

using namespace cc;
using namespace cc::ui;

class MainView: public View
{
    friend class Object;

    MainView()
    {
        size = Size{640, 480};
        color = Color{"#FFFFFF"};

        glyphRun_ =
            GlyphRun::layout(
                "Es war einmal eine Königstochter, die ging hinaus in den Wald und setzte "
                "sich an einen kühlen Brunnen. Sie hatte eine goldene Kugel, die war ihr "
                "liebstes Spielwerk, die warf sie in die Höhe und fing sie wieder in der "
                "Luft und hatte ihre Lust daran. Einmal war die Kugel gar hoch geflogen, "
                "sie hatte die Hand schon ausgestreckt und die Finger gekrümmt, um sie "
                "wieder zufangen, da schlug sie neben vorbei auf die Erde, rollte und rollte "
                "und geradezu in das Wasser hinein.",
                Font::select("sans", fontSize_)
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
        p->setSource(Color{"#E0F0FF"});
        p->rectangle(Point{0, 0}, Size{margins_, size()[1]});
        p->rectangle(Point{size()[0]-margins_, 0}, Size{margins_, size()[1]});
        p->fill();
        p->setSource(Color{"#000000"});
        p->showGlyphRun(Point{margins_, margins_ + fontSize_}, elidedGlyphRun_);
    }

    const double margins_ = 30;
    const double fontSize_ = 20;
    Ref<const GlyphRun> glyphRun_;
    Ref<const GlyphRun> elidedGlyphRun_;
};

int main(int argc, char **argv)
{
    Application *app = Application::open(argc, argv);
    String fontPath = "/usr/share/fonts/truetype/dejavu/";
    if (argc > 1) fontPath = argv[1];
    FontManager::instance()->addPath(fontPath);
    Window::open(Object::create<MainView>(), fontPath);
    return app->run();
}
