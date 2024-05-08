#include <cc/FilledButton>
#include <cc/Label>
#include <cc/debugging>

int main() {
    using namespace cc;

    View view;
    FilledButton button;
    Property<String> text;

    return
        View{sp(640), sp(480)}
        .associate(&view)
        .add(
            FilledButton{"Click me!"}
            .associate(&button)
            .onClicked([=] mutable { text = "Hello, bindings!"; })
            .center([=]{ return view.size() / 2; })
        )
        .add(
            Label{}
            .text([=]{ return text; })
            .centerLeft([=]{
                return button.centerRight() + Point{button.height() / 2, 0};
            })
        )
        .run();
}
