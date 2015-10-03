/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/testing/TestSuite>
#include <flux/stdio>
#include <flux/Property>

using namespace flux;
using namespace flux::testing;

int changedNotices = 0;

class Shape: public Object, public Recipient
{
public:
    static Ref<Shape> create() { return new Shape; }

    Property<String> name;
    Property<int> x;
    Property<int> y;

protected:
    Shape(): x(0), y(0) {
        fout() << "Shape::Shape()" << nl;
        name->valueChanged()->connect(this, &Shape::onNameChanged);
    }
    ~Shape() {
        fout() << "Shape::~Shape()" << nl;
    }

private:
    void onNameChanged(String newName) {
        fout("name = \"%%\"\n") << name;
        ++changedNotices;
    }
};

class Observer: public Object, public Recipient
{
public:
    static Ref<Observer> create(Shape *shape) { return new Observer(shape); }

private:
    Observer(Shape *shape) {
        fout() << "Observer::Observer()" << nl;
        shape->x->valueChanged()->connect(this, &Observer::onXChanged);
        shape->y->valueChanged()->connect(this, &Observer::onYChanged);
    }

    ~Observer() {
        fout() << "Observer::~Observer()" << nl;
    }

    void onXChanged(int value) {
        fout("x = %%\n") << value;
        ++changedNotices;
    }
    void onYChanged(int value) {
        fout("y = %%\n") << value;
        ++changedNotices;
    }
};

class ChangedNotices: public TestCase
{
    void run()
    {
        Ref<Shape> shape = Shape::create();
        Ref<Observer> observer = Observer::create(shape);
        shape->name = "circle1";
        shape->x = 7;
        shape->y = 93;
        shape->y = 93;
        observer = 0;
        shape->x = 8;
        FLUX_VERIFY(changedNotices == 3);
    }
};

int main(int argc, char **argv)
{
    FLUX_TESTSUITE_ADD(ChangedNotices);

    return testSuite()->run(argc, argv);
}
