#include <cc/ui/Application>
#include <cc/ui/FontManager>
#include <cc/DEBUG>

int main(int argc, char **argv)
{
    using namespace cc;
    using namespace cc::ui;

    const double fontSize = 20;

    String fontPath = "<default font>";
    if (argc > 1) {
        fontPath = argv[1];
        FontManager::instance()->addPath(fontPath);
    }

    List< Ref<const GlyphRun> > glyphRuns;
    {
        String pangram = "the quick brown fox jumps over the lazy dog";
        pangram = pangram->toUpper() + " / " + pangram;

        for (auto family: FontManager::instance()->getFontFamilies())
        {
            String text = Format{"%% (%%)"} << pangram << family->name();

            // CC_INSPECT(family->name());

            if (family->name()->split(" ")->count() > 2) continue;
                // skip special regions fonts

            Font font{family->name()};
            glyphRuns << GlyphRun::typeset(text, font);

            font << Weight::Bold;
            glyphRuns << GlyphRun::typeset(text, font);

            font << Weight::Normal << Slant::Italic;
            glyphRuns << GlyphRun::typeset(text, font);
        }
    }

    View view{640, 480};

    view->paint >>[=]{
        Painter p{view};
        p->setSource(Color::Black);
        int i = 0;
        for (const GlyphRun *glyphRun: glyphRuns) {
            p->showGlyphRun(
                Step{30, 30 + glyphRun->size()[1] + 2 * fontSize * i++},
                glyphRun
            );
        }
    };

    Window::open(view, fontPath, WindowMode::Accelerated|WindowMode::VSync);

    return Application{}->run();
}
