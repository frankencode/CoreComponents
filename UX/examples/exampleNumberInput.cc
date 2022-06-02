#include <cc/NumberInput>
#include <cc/DEBUG>

int main()
{
    using namespace cc;

    NumberInput input;

    return
        View{sp(320), sp(240)}
        .add(
            NumberInput{}
            .associate(&input)
            .min(-100)
            .max(100)
            .precision(0.1)
            .onAccepted([input]() mutable {
                CC_INSPECT(input.value());
            })
            .focus(true)
            .paper(theme().inputFieldFillColor())
            .centerInParent()
        )
        .run();
}
