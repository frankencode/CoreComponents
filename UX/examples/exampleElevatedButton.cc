#include <cc/ElevatedButton>

int main()
{
    using namespace cc;

    return
        View{sp(500), sp(500)}
        .add(
            ElevatedButton{}
            .icon(Ideographic::Face)
            .text("Press me!")
            .centerInParent()
        )
        .run();
}
