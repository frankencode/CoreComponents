#include <cc/ui/Application>
#include <cc/ui/FontManager>
#include <cc/ui/TextRun>

using namespace cc;
using namespace cc::ui;

class TestView: public View
{
    friend class Object;

    TestView():
        textRun_(TextRun::create())
    {
        size = Size{640, 480};
        color = Color{"#FFFFFF"};

        textRun_->append(
            "Monospaced text, ",
            TextStyle::create(
                Font::select("mono", fontSize_)
            )
        );

        textRun_->append(
            "variable width text, ",
            TextStyle::create(
                Font::select("sans", fontSize_)
            )
        );

        textRun_->append(
            "bold text, ",
            TextStyle::create(
                Font::select("sans", fontSize_, Weight::Bold)
            )
        );

        textRun_->append(
            "small text, ",
            TextStyle::create(
                Font::select("sans", fontSize_ * 0.75)
            )
        );

        textRun_->append(
            "big text, ",
            TextStyle::create(
                Font::select("sans", fontSize_ * 1.25)
            )
        );

        textRun_->append(
            "red text, ",
            TextStyle::create(
                Font::select("sans", fontSize_),
                Color(0x80, 0x00, 0x00)
            )
        );

        textRun_->append(
            "green text, ",
            TextStyle::create(
                Font::select("sans", fontSize_),
                Color(0x00, 0x80, 0x00)
            )
        );

        textRun_->append(
            "blue text, ",
            TextStyle::create(
                Font::select("sans", fontSize_),
                Color(0x00, 0x00, 0x80)
            )
        );

        textRun_->append(
            "underlined text, ",
            TextStyle::create(
                Font::select("sans", fontSize_),
                TextDecoration::Underline
            )
        );

        textRun_->append(
            "struck out text",
            TextStyle::create(
                Font::select("sans", fontSize_),
                TextDecoration::StrikeOut
            )
        );

        textRun_->setTextAlign(TextAlign::Right);
        // textRun_->setTextAlign(TextAlign::Center);

        auto updateTextWrap = [=]{
            wrappedTextRun_ = textRun_->wrap(size()[0] - 2 * margins_);
        };

        updateTextWrap();

        size->connect(updateTextWrap);
    }

    void paint() override
    {
        Painter p(this);
        p->setSource(Color{"#E0F0FF"});
        p->rectangle(Point{0, 0}, Size{margins_, size()[1]});
        p->rectangle(Point{size()[0]-margins_, 0}, Size{margins_, size()[1]});
        p->fill();
        p->setSource(Color{"#000000"});
        // p->showTextRun(Point{margins_, margins_ + fontSize_}, textRun_);
        p->showTextRun(Point{margins_, margins_ + fontSize_}, wrappedTextRun_);
    }

    const double margins_ = 30;
    const double fontSize_ = 20;
    Ref<TextRun> textRun_;
    Ref<TextRun> wrappedTextRun_;
};

int main(int argc, char **argv)
{
    Application *app = Application::open(argc, argv);
    String fontPath = "/usr/share/fonts/truetype/dejavu/";
    if (argc > 1) fontPath = argv[1];
    FontManager::instance()->addPath(fontPath);
    Window::open(Object::create<TestView>(), fontPath);
    return app->run();
}
