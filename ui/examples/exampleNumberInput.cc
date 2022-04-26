#include <cc/Application>
#include <cc/NumberInput>
#include <cc/DEBUG>

int main()
{
    using namespace cc;

    NumberInput input;

    Window{
        View{sp(320), sp(240)}
        .add(
            NumberInput{&input}
            .min(-100)
            .max(100)
            .precision(0.1)
            .onAccepted([input]{
                CC_INSPECT(input.value());
            })
            .paper(theme().inputFieldFillColor())
            .centerInParent()
        )
    }.show();

    input.focus(true);

    return Application{}.run();
}
