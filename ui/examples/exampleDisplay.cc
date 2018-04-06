#include <cc/stdio>
#include <cc/debug>
#include <cc/ui/Application>
#include <cc/ui/DisplayManager>

using namespace cc;
using namespace cc::ui;

int main(int argc, char **argv)
{
    Application::open(argc, argv);
    auto dm = DisplayManager::instance();
    CC_INSPECT(dm->displayDensityRatio());
    for (int i = 0, n = dm->getDisplayCount(); i < n; ++i) {
        auto d = dm->getDisplay(i);
        fout()
            << "Display " << i << ":" << nl
            << "  pos : " << d->pos() << nl
            << "  size: " << d->size() << nl
            << "  dpi : " << d->dpi() << nl;
    }

    return 0;
}
