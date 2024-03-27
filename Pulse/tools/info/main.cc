#include <cc/PulseContext>
#include <cc/stdio>

int main(int argc, char *argv[])
{
    using namespace cc;

    String toolName = String{argv[0]}.baseName();

    PulseMainLoop mainLoop;

    PulseContext context { mainLoop, toolName };

    context.connect([&]{
        context.requestServerInfo([&](const PulseServerInfo &info)
        {
            fout()
                << info.serverName() << nl
                << info.serverVersion() << nl
                << info.defaultSinkName() << nl
                << info.defaultSourceName() << nl;

            mainLoop.quit();
        });
    });

    return mainLoop.run();
}
