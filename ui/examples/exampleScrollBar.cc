#include <cc/ScrollBar>

int main()
{
    using namespace cc;

    return
        Control{gu(80), gu(60)}
        .add(
            ScrollBar{}
            .visibleOffset(-gu(30))
            .totalExtent(gu(120))
            .autoHide(true)
        )
        .run();
}
