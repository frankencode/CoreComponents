#include <cc/ui/Application>
#include <cc/ui/GlyphRun>
#include <cc/ui/Painter>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    const double fontSize = 20;
    const double margins = 30;

    GlyphRun glyphRun{
        "Es war einmal eine Königstochter, die ging hinaus in den Wald und setzte "
        "sich an einen kühlen Brunnen. Sie hatte eine goldene Kugel, die war ihr "
        "liebstes Spielwerk, die warf sie in die Höhe und fing sie wieder in der "
        "Luft und hatte ihre Lust daran. Einmal war die Kugel gar hoch geflogen, "
        "sie hatte die Hand schon ausgestreckt und die Finger gekrümmt, um sie "
        "wieder zufangen, da schlug sie neben vorbei auf die Erde, rollte und rollte "
        "und geradezu in das Wasser hinein.",
        Font{fontSize}
    };

    View view;

    Window{
        View{&view}
        .size(640, 480)
        .paint([=]{
            GlyphRun elidedRun = glyphRun.elide(view.size()[0] - 2 * margins);

            Painter p{view};
            p.setPen(0xE0F0FF);
            p.rectangle(Point{0, 0}, Size{margins, view.size()[1]});
            p.rectangle(Point{view.size()[0] - margins, 0}, Size{margins, view.size()[1]});
            p.fill();
            p.setPen(0x000000);
            if (elidedRun) p.showGlyphRun(Point{margins, margins + fontSize}, elidedRun);
        })
    }.show();

    return Application{}.run();
}
