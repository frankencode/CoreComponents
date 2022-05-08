#include <cc/StackView>
#include <cc/Control>
#include <cc/stdio>

int main()
{
    using namespace cc;

    StackView stack;

    return
        StackView{sp(100), sp(100), &stack}
        .push(
            Control{}
            .onClicked([stack]() mutable {
                stack.push(
                    Control{}
                    .onClicked([stack]() mutable {
                        stack.pop();
                    })
                    .paper("magenta")
                    .onEndOfLife([]{ ferr() << "Done.2" << nl; })
                );
            })
            .size(sp(100), sp(100))
            .paper("aliceblue")
            .onEndOfLife([]{ ferr() << "Done.1" << nl; })
        )
        .run();
}
