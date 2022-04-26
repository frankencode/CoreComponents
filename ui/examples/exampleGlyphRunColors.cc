#include <cc/Application>
#include <cc/GlyphRun>
#include <cc/Painter>
#include <cc/Random>

int main()
{
    using namespace cc;

    // Application{}.fontSmoothing(FontSmoothing::RgbSubpixel);

    const double fontSize = 20;
    const double margins = fontSize;

    GlyphRun run = {
        "Es war einmal eine Königstochter, die ging hinaus in den Wald und setzte "
        "sich an einen kühlen Brunnen. Sie hatte eine goldene Kugel, die war ihr "
        "liebstes Spielwerk, die warf sie in die Höhe und fing sie wieder in der "
        "Luft und hatte ihre Lust daran. Einmal war die Kugel gar hoch geflogen, "
        "sie hatte die Hand schon ausgestreckt und die Finger gekrümmt, um sie "
        "wieder zufangen, da schlug sie neben vorbei auf die Erde, rollte und rollte "
        "und geradezu in das Wasser hinein.",
        Font{Pitch::Fixed, fontSize}
    };

    Random random{0};
    Array<Color> fgColors = Array<Color>::allocate(run.text().count());
    Array<Color> bgColors = Array<Color>::allocate(run.text().count());

    Color fg, bg;
    for (int i = 0; i < run.text().count(); ++i) {
        if (!fg) {
            int angle = random.get(0, 360);
            fg = Color::fromHue(angle);
            bg = Color::fromHue((angle + 180) % 360);
        }
        fgColors[i] = fg;
        bgColors[i] = bg;
        if (run.text()[i] == ' ') fg = Color{};
    }

    View view;

    Window{
        View{&view}
        .size(640, 480)
        .paper(Color::White)
        .paint([=]{
            GlyphRun wrappedRun = run.wrap(view.size()[0] - 2 * margins, TextAlign::Justify);
            Painter p{view};
            p.setPen(0xE0F0FF);
            p.rectangle(Point{0, 0}, Size{margins, view.size()[1]});
            p.rectangle(Point{view.size()[0]-margins, 0}, Size{margins, view.size()[1]});
            p.fill();
            p.setPen(0x000000);
            p.showGlyphRun(
                Point{margins, margins + fontSize},
                wrappedRun,
                [=](int i) {
                    if (fgColors.has(i)) return fgColors[i];
                    return Color{};
                },
                [=](int i) {
                    if (bgColors.has(i)) return bgColors[i];
                    return Color{};
                }
            );
        })
    }.show();

    return Application{}.run();
}
