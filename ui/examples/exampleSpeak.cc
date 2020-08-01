#include <cc/ui/Application>
#include <cc/ui/ScrollView>
#include <cc/ui/TextField>
#include <cc/Process>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    Process voice {
        Command{"espeak -v de -s 120"}
        ->simpleRedirection()
    };

    // Application{}->textZoom = 1.2;

    View view{640, 480};

    TextField field{"Texteingabe"};
    field->pos = dp(12);
    field->size <<[=]{
        return Size{
            view->size()[0] - 2 * field->pos()[0],
            field->preferredSize()[1]
        };
    };
    field->accepted >>[=]{
        voice->input()->write(field->inputText() + "\n");
    };
    view << field;

    Window::open(view, "Robosprech", WindowMode::Accelerated|WindowMode::VSync);

    return Application{}->run();
}
