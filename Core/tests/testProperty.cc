#include <cc/Property>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "SimpleBindingTest",
        []{
            Property<int> x = 1;
            Property<int> x2 { [=]{ return x * 2; } };
            Property<int> y { &x2 };

            x.onChanged([=]{ CC_INSPECT(x); });
            y.onChanged([=]{ CC_INSPECT(y); });

            for (int i = 0; i < 10; ++i)
                ++x;

            CC_CHECK(x == 11);
            CC_CHECK(y == 22);
        }
    };

    TestCase {
        "ComplexBindingTest",
        []{
            struct Shape
            {
                Property<double> x;
                Property<double> y;
                Property<double> sum { [this]{ return x + y; } };
                Property<double> left { &x };

                void moveTo(double x2, double y2) { x = x2; y = y2; }
            };

            Shape shape;
            auto moved = [=]{
                fout() << "Shape moved to " << shape.x << ", " << shape.y << " (x + y = " << shape.sum << ")" << nl;
                CC_VERIFY(shape.sum() == shape.x + shape.y);
            };
            shape.x.onChanged(moved);
            shape.y.onChanged(moved);
            Shape shadow;
            shadow.x.define([=]{ return shape.x + 10; });
            shadow.y.define([=]{ return shape.y + 10; });
            shape.moveTo(10, 20);
            shape.moveTo(11, 30);
            shape.x = shape.y;
            shape.x = 7;
            shape.left = 0;
            fout() << "Shadow followed to " << shadow.x << ", " << shadow.y << " (x + y = " << shadow.sum << ")" << nl;
            CC_CHECK(shadow.x == 10 && shadow.y == 40);
        }
    };

    TestCase {
        "ConstrainingTest",
        []{
            Property<int> x = 0;
            x.restrict([](int &n, int o) -> bool {
                if (n < 1) n = 1;
                else if (n > 10) n = 10;
                return true;
            });
            x.onChanged([=]{ CC_INSPECT(x); });
            x = -1;
            CC_CHECK(x == 1);
            x = 5;
            CC_CHECK(x == 5);
            x = 20;
            CC_CHECK(x == 10);
        }
    };

    TestCase {
        "BindingLoopTest",
        []{
            struct TestObject {
                Property<int> x { [this]{ return y + 1; } };
                Property<int> y { [this]{ return x + 1; } };
            };
            try {
                TestObject{}.x();
                CC_VERIFY(false);
            }
            catch (PropertyBindingError &ex) {
                CC_INSPECT(ex);
            };
        }
    };

    return TestSuite{argc, argv}.run();
}
