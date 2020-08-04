#include <cc/ui/Application>
#include <cc/ui/FontManager>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    const double fontSize = 20;
    const double margins = 30;

    View view{640, 480};

    auto glyphRun =
        GlyphRun::typeset(
            "Es war einmal eine Königstochter, die ging hinaus in den Wald und setzte "
            "sich an einen kühlen Brunnen. Sie hatte eine goldene Kugel, die war ihr "
            "liebstes Spielwerk, die warf sie in die Höhe und fing sie wieder in der "
            "Luft und hatte ihre Lust daran. Einmal war die Kugel gar hoch geflogen, "
            "sie hatte die Hand schon ausgestreckt und die Finger gekrümmt, um sie "
            "wieder zufangen, da schlug sie neben vorbei auf die Erde, rollte und rollte "
            "und geradezu in das Wasser hinein.",
            Font{fontSize}
        );

    view->paint <<[=]{
        auto elidedRun = glyphRun->elide(view->size()[0] - 2 * margins);

        Painter p{view};
        p->setSource(Color{0xE0F0FF});
        p->rectangle(Point{0, 0}, Size{margins, view->size()[1]});
        p->rectangle(Point{view->size()[0] - margins, 0}, Size{margins, view->size()[1]});
        p->fill();
        p->setSource(Color{0x000000});
        p->showGlyphRun(Point{margins, margins + fontSize}, elidedRun);
    };

    Window::open(view, "Hello, world!", WindowMode::Accelerated|WindowMode::VSync);

    return Application{}->run();
}
