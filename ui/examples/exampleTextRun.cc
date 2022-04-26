#include <cc/Application>
#include <cc/Painter>
#include <cc/TextRun>

int main()
{
    using namespace cc;

    const double margins = 20;

    TextRun textRun;
    textRun.append("Monospaced text, ", Font{Pitch::Fixed});
    textRun.append("variable width text, ", Font{});
    textRun.append("bold text, ", Font{}.weight(Weight::Bold));
    textRun.append("small text, ", Font{} * 0.75);
    textRun.append("big text, ", Font{} * 1.25);
    textRun.append("red text, ", Font{}.color(Color::Red));
    textRun.append("green text, ", Font{}.color(Color::Green));
    textRun.append("blue text, ", Font{}.color(Color::Blue));
    textRun.append("underlined text, ", Font{}.decoration(Decoration::Underline));
    textRun.append("struck out text, ", Font{}.decoration(Decoration::StrikeOut));
    textRun.appendHtml(
        "&lt;HTML&gt; <i>italic text</i>, <b>bold text</b>, <u>underlined text</u>, <b><i><strike>bold italic struck out text</strike></i></b>"
    );

    View view;

    Window{
        View{&view}
        .size(640, 480)
        .paper(Color::White)
        .paint([=]{
            const auto size = view.size();
            TextRun wrappedRun = textRun.wrap(size[0] - 2 * margins);

            Painter p{view};
            p.setPen(0xE0F0FF);
            p.rectangle(Point{0, 0}, Size{margins, size[1]});
            p.rectangle(Point{size[0]-margins, 0}, Size{margins, size[1]});
            p.fill();
            p.setPen(0x000000);
            p.showTextRun(Point{margins, margins + style().defaultFont().size() }, wrappedRun);
        })
    }.show();

    return Application{}.run();
}
