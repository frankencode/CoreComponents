#include <cc/pulse/Context>
#include <cc/stdio>

int main(int argc, char *argv[])
{
    using namespace cc;
    using namespace cc::pulse;

    String toolName = String{argv[0]}.baseName();

    MainLoop mainLoop;

    Context context{mainLoop, toolName};

    context.connect([&]{
        context.requestServerInfo([&](const ServerInfo &info)
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
