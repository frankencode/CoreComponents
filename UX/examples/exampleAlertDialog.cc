#include <cc/AlertDialog>

int main()
{
    using namespace cc;

    return
        AlertDialog{}
        .text("Discard draft?")
        .addAction(Action{"Cancel"})
        .addAction(Action{"Discard"})
        .run();
}
