/*
 * Copyright (C) 2007-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/debug>
#include <cc/Property>

using namespace cc;
using namespace cc::testing;

class Shape: public Object
{
public:
    static Ref<Shape> create() { return new Shape(); }

    Property<double> x;
    Property<double> y;
    Property<double> left = alias(x);

    Property<double> sum { [=]{ return x() + y(); } };

    void moveTo(double x2, double y2) { x = x2; y = y2; }

protected:
    Shape() {
        auto moved = [=]{
            fout() << "Shape moved to " << x() << ", " << y() << " (x + y = " << sum() << ")" << nl;
            CC_VERIFY(sum() == x() + y());
        };
        x->connect(moved);
        y->connect(moved);
    }
};

class BasicBindingsTest: public TestCase
{
    void run() override
    {
        Ref<Shape> shape = Shape::create();
        Ref<Shape> shadow = Shape::create();
        shadow->x->bind([=]{ return shape->x() + 10; });
        shadow->y->bind([=]{ return shape->y() + 10; });
        shape->moveTo(10, 20);
        shape->moveTo(11, 30);
        shape->x = shape->y;
        shape->left = 7;
        shape->left = false;
        fout() << "Shadow followed to " << shadow->x() << ", " << shadow->y() << " (x + y = " << shadow->sum() << ")" << nl;
        CC_VERIFY(shadow->x() == 10 && shadow->y() == 40);
    }
};

class GuardTest: public TestCase
{
    void run() override
    {
        Property<int> x = 0;
        x->setGuard([](int o, int n) -> int {
            if (n < 1) n = 1;
            else if (n > 10) n = 10;
            return n;
        });
        x->connect([&]{ CC_INSPECT(x()); });
        x = -1;
        CC_VERIFY(x() == 1);
        x = 5;
        CC_VERIFY(x() == 5);
        x = 20;
        CC_VERIFY(x() == 10);
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(BasicBindingsTest);
    CC_TESTSUITE_ADD(GuardTest);
    return TestSuite::instance()->run(argc, argv);
};
