#include <cc/stdio>
#include <cc/PluginManager>
#include <cc/ui/types>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    for (auto pluginDir: PluginManager::instance()->getPluginDirs()) {
        fout() << "Plugin dir " << pluginDir->path() << ":" << nl;
        for (auto plugin: pluginDir->getLoadedPlugins())
            fout() << "  Plugin " << plugin->name() << nl;
    }
    Point p { 10, 10 };
    fout() << "p = " << p << nl;
    return 0;
}
