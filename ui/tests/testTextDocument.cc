#include <cc/debug>
#include <cc/testing/TestSuite>
#include <cc/ui/TextDocument>

using namespace cc;
using namespace cc::ui;
using namespace cc::testing;

class SingleLineTest: public TestCase
{
    void run() override
    {
        auto document = Object::create<TextDocument>();
        CC_INSPECT(document->text());
        CC_VERIFY(document->text() == "");
        document->paste(Range{0, 0}, "Hello, world!\n");
        CC_INSPECT(document->text());
        CC_VERIFY(document->text() == "Hello, world!\n");
        document->paste(Range{7, 12}, "Joe");
        CC_INSPECT(document->text());
        CC_VERIFY(document->text() == "Hello, Joe!\n");
        CC_INSPECT(document->canUndo());
        CC_VERIFY(document->canUndo());
        document->undo();
        CC_INSPECT(document->text());
        CC_VERIFY(document->text() == "Hello, world!\n");
        CC_INSPECT(document->canRedo());
        CC_VERIFY(document->canRedo());
        document->redo();
        CC_INSPECT(document->text());
        CC_VERIFY(document->text() == "Hello, Joe!\n");
        document->undo();
        CC_INSPECT(document->text());
        CC_INSPECT(document->canUndo());
        document->undo();
        CC_INSPECT(document->text());
        CC_INSPECT(document->canUndo());
            // TODO: review history logic (separate test case)

        document->paste(Range{0, document->byteCount()}, "");
        CC_INSPECT(document->text());
        CC_VERIFY(document->text() == "");
    }
};

class MultiLineTest: public TestCase
{
    void run() override
    {
        auto document = Object::create<TextDocument>();
        document->paste(Range{0},
            "#include <stdio.h>\n"
            "void main() {\n"
            "    printf(\"Be damned!\");\n"
            "    return 0;\n"
            "}\n"
        );
        CC_INSPECT(document->text());
        {
            int j = document->text()->find("main()");
            CC_INSPECT(j);
            document->paste(Range{j+5}, "int argc, char **argv");
        }
        CC_INSPECT(document->text());
        CC_VERIFY(document->text() ==
            "#include <stdio.h>\n"
            "void main(int argc, char **argv) {\n"
            "    printf(\"Be damned!\");\n"
            "    return 0;\n"
            "}\n"
        );
        {
            int j = document->text()->find("Be damned!");
            CC_INSPECT(j);
            Range r = document->paste(Range{j, j + 10}, "argc = %d\\n");
            document->paste(Range{r->i1() + 1}, ", argc");
        }
        CC_INSPECT(document->text());
        CC_VERIFY(document->text() ==
            "#include <stdio.h>\n"
            "void main(int argc, char **argv) {\n"
            "    printf(\"argc = %d\\n\", argc);\n"
            "    return 0;\n"
            "}\n"
        );
    };
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(SingleLineTest);
    CC_TESTSUITE_ADD(MultiLineTest);

    return TestSuite::instance()->run(argc, argv);
}
