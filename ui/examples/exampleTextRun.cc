#include <cc/TextRun>
#include <cc/View>

int main()
{
    using namespace cc;

    const double margins = 20;

    auto textRun =
        TextRun{}
        .append("Monospaced text, ", Font{Pitch::Fixed})
        .append("variable width text, ", Font{})
        .append("bold text, ", Font{}.weight(Weight::Bold))
        .append("small text, ", Font{} * 0.75)
        .append("big text, ", Font{} * 1.25)
        .append("red text, ", Font{}.color(Color::Red))
        .append("green text, ", Font{}.color(Color::Green))
        .append("blue text, ", Font{}.color(Color::Blue))
        .append("underlined text, ", Font{}.decoration(Decoration::Underline))
        .append("struck out text, ", Font{}.decoration(Decoration::StrikeOut))
        .appendHtml(
            "&lt;HTML&gt; "
            "<i>italic text</i>, "
            "<b>bold text</b>, "
            "<u>underlined text</u>, "
            "<b><i><strike>bold italic struck out text</strike></i></b>"
        );

    View view;

    return
        View{&view}
        .size(sp(500), sp(500))
        .paper(Color::White)
        .paint([=]{
            const auto size = view.size();
            TextRun wrappedRun = textRun.wrap(size[0] - 2 * margins);

            Painter{view}
            .setPen(0xE0F0FF)
            .rectangle(Point{0, 0}, Size{margins, size[1]})
            .rectangle(Point{size[0]-margins, 0}, Size{margins, size[1]})
            .fill()
            .setPen(0x000000)
            .showTextRun(Point{margins, margins + style().defaultFont().size() }, wrappedRun);
        })
        .run();
}
