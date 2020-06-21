#include <cc/ui/DisplayManager>
#include <cc/stdio>
#include <cc/DEBUG>

using namespace cc;
using namespace cc::ui;

int main(int argc, char **argv)
{
    auto dm = DisplayManager::instance();
    CC_INSPECT(dm->displayDensityRatio());
    CC_INSPECT(dm->defaultFontSmoothing());
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
