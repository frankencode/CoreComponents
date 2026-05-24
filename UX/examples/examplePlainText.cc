#include <cc/PlainText>
#include <cc/Random>
#include <cc/View>

int main()
{
    using namespace cc;

    const double fontSize = 18;
    const double margin = fontSize;

    const String text =
        "Es war einmal eine Königstochter, die ging hinaus in den Wald und setzte "
        "sich an einen kühlen Brunnen. Sie hatte eine goldene Kugel, die war ihr "
        "liebstes Spielwerk, die warf sie in die Höhe und fing sie wieder in der "
        "Luft und hatte ihre Lust daran. Einmal war die Kugel gar hoch geflogen, "
        "sie hatte die Hand schon ausgestreckt und die Finger gekrümmt, um sie "
        "wieder zufangen, da schlug sie neben vorbei auf die Erde, rollte und rollte "
        "und geradezu in das Wasser hinein.";

    Random random{0};
    Array<Color> fgColors = Array<Color>::allocate(text.count());
    Array<Color> bgColors = Array<Color>::allocate(text.count());

    Color fg, bg;
    for (int i = 0; i < text.count(); ++i) {
        if (!fg) {
            int angle = random.get(0, 360);
            fg = Color::fromHue(angle);
            bg = Color::fromHue((angle + 180) % 360);
        }
        fgColors[i] = fg;
        bgColors[i] = bg;
        if (text[i] == ' ') fg = Color{};
    }

    PlainText view;

    return
        PlainText{text, Font{Pitch::Fixed, fontSize}}
        .associate(&view)
        .size(sp(500))
        .paper(Color::White)
        .margin(margin)
        .maxWidth([=]{ return view.width() - 2 * margin; })
        .getInk(
            [=](int i) {
                if (fgColors.has(i)) return fgColors[i];
                return Color{};
            }
        )
        .getPaper(
            [=](int i) {
                if (bgColors.has(i)) return bgColors[i];
                return Color{};
            }
        )
        .run();
}
