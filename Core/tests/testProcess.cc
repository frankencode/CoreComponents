#include <cc/Process>
#include <cc/Command>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "SimpleSpawning",
        []{
            String execPath = Process::execPath();
            CC_INSPECT(execPath);
            int ret = Process{execPath + " test"}.wait();
            CC_CHECK(ret == 7);
        }
    };

    TestCase
    {
        "SimpleEcho",
        []{
            String execPath = Process::execPath();
            Process process{execPath + " echo"};
            String message = "Hello, echo!";
            process.input().write(message);
            process.input().close();
            String reply = process.output().readAll();
            CC_INSPECT(message);
            CC_INSPECT(reply);
            CC_CHECK(message == reply);
            int ret = process.wait();
            CC_CHECK(ret == 11);
        }
    };

    if (argc == 2) {
        if (String{argv[1]} == "test") {
            fout() << "Hello, echo!" << nl;
            return 7;
        }
        if (String{argv[1]} == "echo") {
            stdOutput().write(stdInput().readAll());
            return 11;
        }
    }

    return TestSuite{argc, argv}.run();
}
