#include <cc/Variant>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "SimpleValueStorageAndRetrieval",
        []{
            Variant a = 1;
            Variant b = 2.3;
            Variant c = "123";
            Variant d = true;

            CC_INSPECT(a);
            CC_INSPECT(b);
            CC_INSPECT(c);
            CC_INSPECT(d);

            CC_CHECK(a.is<long>());
            CC_CHECK(b.is<double>());
            CC_CHECK(c.is<String>());
            CC_CHECK(d.is<bool>());

            CC_CHECK(String{a.typeName()} == "long");

            CC_CHECK(a.to<long>() == 1);
            CC_CHECK(static_cast<bool>(a) == true);
            CC_CHECK(static_cast<double>(a) == 1.);
            CC_CHECK(static_cast<double>(b) == 2.3);
            CC_CHECK(static_cast<String>(c) == "123");

            a = 2;
            a = static_cast<long>(a) + 1;

            CC_CHECK(a.to<long>() == 3);
            CC_INSPECT(sizeof(Variant));
            CC_CHECK(sizeof(Variant) <= 16);
        }
    };


    TestCase {
        "ComplexValueStorageAndRetrieval",
        []{
            Variant l = List<Variant>{"A", "B", "C"};
            CC_INSPECT(l);

            Variant m { "x", "y", "z" };
            CC_INSPECT(m);
            CC_INSPECT(m.typeName());
            CC_VERIFY(m.is<List<String>>());

            Variant n { 1, 2, 3 };
            CC_INSPECT(n);
            CC_INSPECT(n.typeName());
            CC_VERIFY(n.is<List<long>>());

            Variant o { 1.1, 2.2, 3.3 };
            CC_INSPECT(o);
            CC_INSPECT(o.typeName());
            CC_VERIFY(o.is<List<double>>());

            Variant p { 1.1 };
            CC_INSPECT(p);
            CC_INSPECT(p.typeName());
            CC_VERIFY(p.is<double>());
        }
    };


    return TestSuite{argc, argv}.run();
}
