#include <cc/pulse/Context>
#include <cc/pulse/InputStream>
#include <cc/stdio>
#include <cc/hexdump>
#include <cc/DEBUG>

int main(int argc, char *argv[])
{
    using namespace cc;
    using namespace cc::pulse;

    String toolName = String{argv[0]}.baseName();

    MainLoop mainLoop;

    Context context{mainLoop, toolName};
    InputStream stream{context};

    context.connect([&]{
        context.requestServerInfo([&](const ServerInfo &info)
        {
            String target = info.defaultSinkName() + ".monitor";
            CC_INSPECT(target);

            stream.incoming([](const Bytes &data){
                fout() << hexdump(data);
            });

            stream.connect(target);
        });
    });

    return mainLoop.run();
}
