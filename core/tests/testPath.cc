#include <cc/Process>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "BasicManipulation",
        []{
            String execPath = TestSuite{}.execPath();
            CC_INSPECT(execPath);
            CC_INSPECT(execPath.fileName());
            CC_INSPECT(execPath.isAbsolutePath());
            CC_INSPECT(Process::cwd().cd(execPath));
            {
                String path{Process::cwd()};
                path = path.cd(execPath);
                while (!path.isRootPath()) {
                    path = path.cdUp();
                    CC_INSPECT(path);
                }
            }
            CC_CHECK(String{"/"} / "/usr" == "/usr");
        }
    };

    return TestSuite{argc, argv}.run();
}
