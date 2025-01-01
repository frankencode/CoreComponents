#include <cc/paint/Canvas>

int main(int argc, char *argv[])
{
    using namespace cc::paint;
    using namespace cc;

    return
        Canvas{sp(1000), sp(800)}
        .run();
}
