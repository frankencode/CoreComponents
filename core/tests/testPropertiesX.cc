/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/PropertyX>

using namespace cc;
using namespace cc::testing;

class Shape: public Object
{
public:
    static Ref<Shape> create() { return new Shape(); }

    PropertyX<double> x;
    PropertyX<double> y;

    void moveTo(double x2, double y2) { x = x2; y = y2; }

protected:
    Shape(): x(0), y(0) {}
};

int main(int argc, char **argv)
{
    Ref<Shape> shape = Shape::create();
    auto shapeMoved = [=]{ fout() << "Moved to " << shape->x << ", " << shape->y << nl; };
    shape->x->valueChanged->connect(shapeMoved);
    shape->y->valueChanged->connect(shapeMoved);
    shape->moveTo(10, 20);
    shape->moveTo(11, 30);
    fout() << sizeof(shape->x) << nl;
    return 0;
};
