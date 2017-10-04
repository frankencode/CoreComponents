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
#include <cc/PropertyX>

using namespace cc;
using namespace cc::testing;

class Shape: public Object
{
public:
    static Ref<Shape> create() { return new Shape(); }

    PropertyX<double> x;
    PropertyX<double> y;
    PropertyX<double> x0 = alias(x);

    PropertyX<double> sum { [=]{ return x + y; } };

    void moveTo(double x2, double y2) { x = x2; y = y2; }

protected:
    Shape(): x(0), y(0) {}
};

int main(int argc, char **argv)
{
    Ref<Shape> shape = Shape::create();
    Ref<Shape> shadow = Shape::create();
    shadow->x = [=]{ return shape->x + 10; };
    shadow->y = [=]{ return shape->y + 10; };
    auto moved = [=]{ fout() << "Shape moved to " << shape->x << ", " << shape->y << " (x + y = " << shape->sum << ")" << nl; };
    shape->x->valueChanged->connect(moved);
    shape->y->valueChanged->connect(moved);
    shape->moveTo(10, 20);
    shape->moveTo(11, 30);
    shape->x = shape->y;
    shape->x0 = 7;
    shape->x0 = false;
    fout() << "Shadow followed to " << shadow->x << ", " << shadow->y << " (x + y = " << shadow->sum << ")" << nl;
    CC_INSPECT(sizeof(shape->x));
    return 0;
};
