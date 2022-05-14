#include <cc/GlyphRun>
#include <cc/View>

int main()
{
    using namespace cc;

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

    return
        View{}
        .associate(&view)
        .size(sp(500), sp(500))
        .paint([=]{
            GlyphRun elidedRun = glyphRun.elide(view.size()[0] - 2 * margins);

            Painter{view}
            .setPen(0xE0F0FF)
            .rectangle(Point{0, 0}, Size{margins, view.size()[1]})
            .rectangle(Point{view.size()[0] - margins, 0}, Size{margins, view.size()[1]})
            .fill()
            .setPen(0x000000)
            .showGlyphRun(Point{margins, margins + fontSize}, elidedRun);
        })
        .run();
}
