#include <cc/paint/Canvas>

int main(int argc, char *argv[])
{
    using namespace cc::paint;
    using namespace cc;

    return
        Canvas{sp(800), sp(600)}
        .run();
}
