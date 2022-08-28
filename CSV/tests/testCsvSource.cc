#include <cc/CsvSource>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "CsvSourceSimple",
        []{
            const String text {
                "name,type,value,description\n"
                "speed,int,50,\"Speed in km/h\"\n"
                "phi,double,0.0,\"Angular velocity\n"
                                 "Also known as \"\"wheel speed\"\"\n"
                                 "Rotation speed around major axis\"\n"
            };

            List<List<String>> table;
            CsvSource source{text};
            for (const List<String> &record: source) {
                table.append(record);
                for (const String &item: record) {
                    ferr() << item << (record.isLast(item) ? "\n" : ", ");
                }
            }

            CC_CHECK(table(0)(0) == "name");
            CC_CHECK(table(0)(3) == "description");
            CC_CHECK(table(1)(1) == "int");
            CC_CHECK(table(1)(2) == "50");
            CC_CHECK(table(2)(3).startsWith("Angular"));
            CC_CHECK(table(2)(3).endsWith("axis"));
            CC_CHECK(table(2)(3).contains('\n'));
            CC_CHECK(table(2)(3).count('"') == 2);
        }
    };

    return TestSuite{argc, argv}.run();
}
