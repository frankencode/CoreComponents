#include <cc/ui/SdlApplication>

using namespace cc;
using namespace cc::ui;

int main(int argc, char **argv)
{
    SdlApplication::instance()->init(argc, argv);
    return 0;
}
