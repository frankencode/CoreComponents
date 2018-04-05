#include <cc/ui/Application>
#include <cc/ui/View>
#include <cc/ui/FontManager>
#include <cc/ui/TextRun>

using namespace cc;
using namespace cc::ui;

class MainView: public View
{
    friend class Object;

    MainView():
        textRun_(TextRun::create())
    {
        size = Size{640, 480};
        color = Color{"#FFFFFF"};

        textRun_->append("Monospaced text, ", Font() << Pitch::Fixed);
        textRun_->append("variable width text, ", Font());
        textRun_->append("bold text, ", Font() << Weight::Bold);
        textRun_->append("small text, ", Font() * 0.75);
        textRun_->append("big text, ", Font() * 1.25);
        textRun_->append("red text, ", Font() << Color("#800000"));
        textRun_->append("green text, ", Font() << Color("#008000"));
        textRun_->append("blue text, ", Font() << Color("#000080"));
        textRun_->append("underlined text, ", Font() << Decoration::Underline);
        textRun_->append("struck out text, ", Font() << Decoration::StrikeOut);
        textRun_->appendHtml(
            "&lt;HTML&gt; <i>italic text</i>, <b>bold text</b>, <u>underlined text</u>, <b><i><strike>bold italic struck out text</strike></i></b>"
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
        p->showTextRun(Point{margins_, margins_ + Font::defaultSize()}, wrappedTextRun_);
    }

    const double margins_ = 30;
    Ref<TextRun> textRun_;
    Ref<TextRun> wrappedTextRun_;
};

int main(int argc, char **argv)
{
    Application *app = Application::open(argc, argv);
    Window::open(Object::create<MainView>(), "Hello, world!");
    return app->run();
}
