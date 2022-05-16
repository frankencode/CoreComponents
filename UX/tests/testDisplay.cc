#include <cc/DisplayManager>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;
    using namespace cc;

    List<Display> displays = DisplayManager{}.displays();
    for (Display display: displays) {
        fout() << display << nl << nl;
    }

    return 0;
}
