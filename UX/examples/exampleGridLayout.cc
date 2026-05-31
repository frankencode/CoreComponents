#include <cc/Label>
#include <cc/layout>

int main()
{
    using namespace cc;

    return
        Grid{}
        (Label{"Device type:"}        )(Label{"ear cleaner"}  )
        (Label{"Serial number:"}      )(Label{"9785916495620"})
        (Label{"Date of manufacture:"})(Label{"20260529"}     )
        .run();
}
