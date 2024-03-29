#include <cc/FontManager>
#include <cc/View>
#include <cc/StylePlugin>
#include <cc/debugging>

int main()
{
    using namespace cc;

    CC_INSPECT(style().defaultFont());
    CC_INSPECT(style().defaultFixedFont());
    CC_INSPECT(style().defaultSmallFont());

    List<GlyphRun> glyphRuns;
    {
        String pangram = "the quick brown fox jumps over the lazy dog";
        pangram = pangram.upcased() + " / " + pangram;

        for (const FontFamily &family: FontManager{}.fontFamilies())
        {
            String text = Format{"%% (%%)"} << pangram << family.name();

            if (!family.name().find("Mono") /*&& !family.name().find("Display")*/) {  // skip some fonts
                if (family.name().find("Music") || family.name().split(" ").count() > 2)
                    continue;
            }

            Font font{family.name()};
            // font.smoothing(FontSmoothing::RgbSubpixel);
            glyphRuns.emplaceBack(text, font);

            font.weight(Weight::Bold);
            glyphRuns.emplaceBack(text, font);

            font.weight(Weight::Normal).slant(Slant::Italic);
            glyphRuns.emplaceBack(text, font);

            for (const FontFace &face: family.fontFaces()) {
                CC_INSPECT(face);
                // CC_INSPECT(face.path());
            }
        }
    }

    View view;

    return
        View{640, 480}
        .associate(&view)
        .paint([=]{
            Painter p{view};
            p.setPen(Color::Black);
            double y = 0;
            for (const GlyphRun &run: glyphRuns) {
                Point pos{30, 30 + run.size()[1] + y};
                p.showGlyphRun(pos.round(),run);
                y += 2 * run.metrics().lineHeight();
            }
        })
        .run();
}
