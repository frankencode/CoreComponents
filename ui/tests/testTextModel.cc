#include <cc/debug>
#include <cc/testing/TestSuite>
#include <cc/ui/TextModel>

using namespace cc;
using namespace cc::ui;
using namespace cc::testing;

class SingleLineTest: public TestCase
{
    void run() override
    {
        auto model = Object::create<TextModel>();
        CC_INSPECT(model->text());
        CC_VERIFY(model->text() == "");
        model->paste(Range{0, 0}, "Hello, world!\n");
        CC_INSPECT(model->text());
        CC_VERIFY(model->text() == "Hello, world!\n");
        model->paste(Range{7, 12}, "Joe");
        CC_INSPECT(model->text());
        CC_VERIFY(model->text() == "Hello, Joe!\n");
        CC_INSPECT(model->canUndo());
        CC_VERIFY(model->canUndo());
        model->undo();
        CC_INSPECT(model->text());
        CC_VERIFY(model->text() == "Hello, world!\n");
        CC_INSPECT(model->canRedo());
        CC_VERIFY(model->canRedo());
        model->redo();
        CC_INSPECT(model->text());
        CC_VERIFY(model->text() == "Hello, Joe!\n");
        model->undo();
        CC_INSPECT(model->text());
        CC_INSPECT(model->canUndo());
        model->undo();
        CC_INSPECT(model->text());
        CC_INSPECT(model->canUndo());
            // TODO: review history logic (separate test case)

        model->paste(Range{0, model->charCount()}, "");
        CC_INSPECT(model->text());
        CC_VERIFY(model->text() == "");
    }
};

class MultiLineTest: public TestCase
{
    void run() override
    {
        auto model = Object::create<TextModel>();
        model->paste(Range{0},
            "#include <stdio.h>\n"
            "void main() {\n"
            "    printf(\"Be damned!\");\n"
            "    return 0;\n"
            "}\n"
        );
        CC_INSPECT(model->text());
        {
            int j = model->text()->find("main()");
            CC_INSPECT(j);
            model->paste(Range{j+5}, "int argc, char **argv");
        }
        CC_INSPECT(model->text());
        CC_VERIFY(model->text() ==
            "#include <stdio.h>\n"
            "void main(int argc, char **argv) {\n"
            "    printf(\"Be damned!\");\n"
            "    return 0;\n"
            "}\n"
        );
        {
            int j = model->text()->find("Be damned!");
            CC_INSPECT(j);
            Range r = model->paste(Range{j, j + 10}, "argc = %d\\n");
            model->paste(Range{r->i1() + 1}, ", argc");
        }
        CC_INSPECT(model->text());
        CC_VERIFY(model->text() ==
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
