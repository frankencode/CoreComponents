#include <cc/ui/Application>
#include <cc/ui/View>
#include <cc/ui/FontManager>

using namespace cc;
using namespace cc::ui;

class MainView: public View
{
    friend class Object;

    MainView()
    {
        size = Size{640, 480};
        color = Color{"#FFFFFF"};

        glyphRuns_ = List< Ref<const GlyphRun> >::create();

        String pangram = "The quick brown fox jumps over the lazy dog";

        for (const FontFamily *fontFamily: FontManager::instance()->getFontFamilies())
        {
            String text = Format("%%: %%") << fontFamily->name() << pangram;

            Font font;
            glyphRuns_ << GlyphRun::typeSet(text, font);

            font->setWeight(Weight::Bold);
            glyphRuns_ << GlyphRun::typeSet(text, font);

            font->setWeight(Weight::Normal);
            font->setSlant(Slant::Italic);
            glyphRuns_ << GlyphRun::typeSet(text, font);
        }
    }

    void paint() override
    {
        Painter p(this);
        p->setSource(Color{"#000000"});
        int i = 0;
        for (const GlyphRun *glyphRun: glyphRuns_) {
            p->showGlyphRun(
                Step{30, 30 + glyphRun->size()[1] + 2 * fontSize_ * i++},
                glyphRun
            );
        }
    }

    const double fontSize_ = 20;
    Ref< List< Ref<const GlyphRun> > > glyphRuns_;
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
