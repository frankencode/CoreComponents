#include <cc/Label>
#include <cc/layout>

int main()
{
    using namespace cc;

    return
        Column{}
        .paper(0xD0D0FF)
        (Label{"• Item 1"})
        (Label{"• Item 2"})
        (Label{"• Item 3"})
        (
            Row{}
            .paper(0xD0FFD0)
            (Label{"• Item A B C"})
            (Label{"• Item B D E"})
            (Label{"• Item C D F"})
        )
        .run();
}
