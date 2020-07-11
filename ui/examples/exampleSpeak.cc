#include <cc/ui/ScrollView>
#include <cc/ui/TextField>
#include <cc/ui/Row>
#include <cc/Process>
#include <cc/debug>

using namespace cc;
using namespace cc::ui;

class MainView: public View
{
    friend class Object;

    MainView()
    {
        voice_ = Process{
            Command{"espeak -v de"}
            ->simpleRedirection()
        };

        // Application{}->textZoom = 1.2;

        size = Size { 640, 480 };
        inheritPaper();

        auto carrier = add<ScrollView>()->carrier();

        auto row = carrier->organize<Row>();
        row->margin = dp(12);
        row->spacing = dp(12);

        auto textField = carrier->add<TextField>("Texteingabe");
        textField->accepted->connect([=]{
            voice_->input()->write(textField->inputText() + "\n");
        });
    }

    ~MainView()
    {
        voice_->kill();
    }

    Process voice_;
};

int main(int argc, char **argv)
{
    Window::open<MainView>("Lesehilfe", WindowMode::Accelerated|WindowMode::VSync);
    return Application{}->run();
}
