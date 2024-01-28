#include <cc/build/DependencyFile>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc::build;
    using namespace cc;

    TestCase {
        "DependencyExampleFile1",
        []{
            String text =
                ".objects-0F0105B9-x86_64-linux-gnu-Base_src/Cow.o \\\n"
                "gcm.cache/cc.core-Cow.gcm: /home/fme/src/CoreComponents/Base/src/Cow.cc\n"
                ".objects-0F0105B9-x86_64-linux-gnu-Base_src/Cow.o \\\n"
                "gcm.cache/cc.core-Cow.gcm: cc.core:Use.c++m\n"
                "cc.core:Cow.c++m: gcm.cache/cc.core-Cow.gcm\n"
                ".PHONY: cc.core:Cow.c++m\n"
                "gcm.cache/cc.core-Cow.gcm:| \\\n"
                ".objects-0F0105B9-x86_64-linux-gnu-Base_src/Cow.o\n"
                "CXX_IMPORTS += cc.core:Use.c++m\n";

            DependencyFile depFile { text };
            CC_CHECK(depFile.isValid());
            CC_CHECK(depFile.sources() == List<String>{ "/home/fme/src/CoreComponents/Base/src/Cow.cc" });
            CC_CHECK(depFile.imports() == List<String>{ "cc.core:Use.c++m" });
            CC_CHECK(depFile.moduleName() == "cc.core:Cow.c++m");
        }
    };

    TestCase {
        "DependencyExampleFile2",
        []{
            String text =
                ".objects-0F0105B9-x86_64-linux-gnu-Base_src/core.o gcm.cache/cc.core.gcm: \\\n"
                " /home/fme/src/CoreComponents/Base/src/core.cc\n"
                ".objects-0F0105B9-x86_64-linux-gnu-Base_src/core.o gcm.cache/cc.core.gcm: \\\n"
                " cc.core:Cow.c++m cc.core:Composite.c++m cc.core:math.c++m \\\n"
                " cc.core:bits.c++m cc.core:Use.c++m\n"
                "cc.core.c++m: gcm.cache/cc.core.gcm\n"
                ".PHONY: cc.core.c++m\n"
                "gcm.cache/cc.core.gcm:| .objects-0F0105B9-x86_64-linux-gnu-Base_src/core.o\n"
                "CXX_IMPORTS += cc.core:Cow.c++m cc.core:Composite.c++m cc.core:math.c++m \\\n"
                " cc.core:bits.c++m cc.core:Use.c++m\n";

            DependencyFile depFile { text };
            CC_CHECK(depFile.isValid());
            CC_CHECK(depFile.sources() == List<String>{ "/home/fme/src/CoreComponents/Base/src/core.cc" });
            CC_CHECK((depFile.imports() == List<String>{ "cc.core:Cow.c++m", "cc.core:Composite.c++m", "cc.core:math.c++m", "cc.core:bits.c++m", "cc.core:Use.c++m" }));
            CC_CHECK(depFile.moduleName() == "cc.core.c++m");
        }
    };

    TestCase {
        "DependencyExampleFile3",
        []{
            String text =
                ".objects-E763DAD2-x86_64-linux-gnu-Core_src/Entity.o: \\\n"
                "/home/fme/src/CoreComponents/Core/src/Entity.cc\n";

            DependencyFile depFile { text };
            CC_CHECK(depFile.isValid());
            CC_CHECK(depFile.sources() == List<String>{ "/home/fme/src/CoreComponents/Core/src/Entity.cc" });
            CC_CHECK(depFile.imports() == List<String>{});
            CC_CHECK(depFile.moduleName() == "");
        }
    };

    return TestSuite{argc, argv}.run();
}
