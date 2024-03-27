#include <cc/PulseContext>
#include <cc/PulseInputStream>
#include <cc/stdio>
#include <cc/HexDump>
#include <cc/DEBUG>

int main(int argc, char *argv[])
{
    using namespace cc;

    String toolName = String{argv[0]}.baseName();

    PulseMainLoop mainLoop;

    PulseContext context { mainLoop, toolName };
    PulseInputStream stream { context };

    context.connect([&]{
        context.requestServerInfo([&](const PulseServerInfo &info)
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
