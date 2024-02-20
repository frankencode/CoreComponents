#include <cc/build/DependencyFile>
#include <cc/LineSource> // DEBUG
#include <cc/Set> // DEBUG
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc::build;
    using namespace cc;

    #if 0
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
    #endif

    TestCase {
        "DependencyExampleFile3",
        []{
            String text =
                ".objects-64C15C06-x86_64-linux-gnu-src_hello_my_module/import_mega.numbers.o \\\n"
                " gcm.cache/mega.numbers.gcm: \\\n"
                " /home/fme/src/hello_my_module/import/mega.numbers.cc \\\n"
                " gcm.cache/usr/local/include/c++/14.0.1/iostream.gcm\n"
                ".objects-64C15C06-x86_64-linux-gnu-src_hello_my_module/import_mega.numbers.o \\\n"
                " gcm.cache/mega.numbers.gcm: /usr/local/include/c++/14.0.1/iostream.c++m\n"
                "mega.numbers.c++m: gcm.cache/mega.numbers.gcm\n"
                ".PHONY: mega.numbers.c++m\n"
                "gcm.cache/mega.numbers.gcm:| \\\n"
                " .objects-64C15C06-x86_64-linux-gnu-src_hello_my_module/import_mega.numbers.o\n"
                "CXX_IMPORTS += /usr/local/include/c++/14.0.1/iostream.c++m\n";

            DependencyFile depFile { text };
            CC_INSPECT(depFile.isValid());
            CC_INSPECT(depFile.sources().join(", "));

            const String sourcePrefix = "/home/fme/src";

            Set<String> deps;
            CC_INSPECT(text.replaced("\\\n", ""));
            for (const String &line: LineSource{text.replaced("\\\n", "")}) {
                List<String> parts = line.split(':');
                if (parts.count() != 2) continue;
                CC_INSPECT(parts(0));
                if (parts(0).contains(".objects-64C15C06-x86_64-linux-gnu-src_hello_my_module/import_mega.numbers.o")) {
                    CC_INSPECT(parts(1));
                    String value = parts(1);
                    value.trim();
                    value.simplify();
                    List<String> list = value.split(' ');
                    for (const String &item: list) {
                        if (item.endsWith(".gcm")) continue;
                        if (item.contains("gcm.cache/")) continue;
                        if (!item.startsWith(sourcePrefix)) continue;
                        CC_INSPECT(item);
                        deps.insert(item);
                    }
                }
            }
            CC_INSPECT(deps.toList().join(", "));
        }
    };

    #if 0
    TestCase {
        "DependencyExampleFile4",
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
    #endif

    return TestSuite{argc, argv}.run();
}
