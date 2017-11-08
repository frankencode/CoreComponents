/*
 * Copyright (C) 2007-2017 Frank Mertens.
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
    Property<double> x0 = alias(x);

    Property<double> sum { [=]{ return x() + y(); } };

    void moveTo(double x2, double y2) { x = x2; y = y2; }

protected:
    Shape(): x(0), y(0) {}
};

class BasicBindings: public TestCase
{
    void run()
    {
        Ref<Shape> shape = Shape::create();
        Ref<Shape> shadow = Shape::create();
        shadow->x->bind([=]{ return shape->x() + 10; });
        shadow->y->bind([=]{ return shape->y() + 10; });
        auto moved = [=]{
            fout() << "Shape moved to " << shape->x() << ", " << shape->y() << " (x + y = " << shape->sum() << ")" << nl;
            CC_VERIFY(shape->sum() == shape->x() + shape->y());
        };
        shape->x->listen(moved);
        shape->y->listen(moved);
        shape->moveTo(10, 20);
        shape->moveTo(11, 30);
        shape->x = shape->y;
        shape->x0 = 7;
        shape->x0 = false;
        fout() << "Shadow followed to " << shadow->x() << ", " << shadow->y() << " (x + y = " << shadow->sum() << ")" << nl;
        CC_VERIFY(shadow->x() == 10 && shadow->y() == 40);
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(BasicBindings);
    return TestSuite::instance()->run(argc, argv);
};
