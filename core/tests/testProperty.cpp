/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio.h>
#include <flux/check.h>
#include <flux/Property.h>

using namespace flux;

int changedNotices = 0;

class Shape: public Object
{
public:
	static Ref<Shape> create() { return new Shape; }

	Property<String> name;
	Property<int> x;
	Property<int> y;

protected:
	Shape()
		: x(0), y(0)
	{
		name->valueChanged()->connect(this, &Shape::onNameChanged);
	}

private:
	void onNameChanged(String newName) {
		fout("name = \"%%\"\n") << name;
		++changedNotices;
	}
};

class Observer: public Object
{
public:
	Observer(Shape *shape)
		: shape_(shape)
	{
		shape_->x->valueChanged()->connect(this, &Observer::onXChanged);
		shape_->y->valueChanged()->connect(this, &Observer::onYChanged);
	}
	~Observer()
	{
		shape_->x->valueChanged()->disconnect(this);
		shape_->y->valueChanged()->disconnect(this);
	}

private:
	void onXChanged(int value) {
		fout("x = %%\n") << value;
		++changedNotices;
	}
	void onYChanged(int value) {
		fout("y = %%\n") << value;
		++changedNotices;
	}

	Ref<Shape> shape_;
};

int main()
{
	Ref<Shape> shape = Shape::create();
	{
		Observer observer(shape);
		shape->name = "circle1";
		shape->x = 7;
		shape->y = 93;
		shape->y = 93;
	}
	shape->x = 8;
	check(changedNotices == 3);
	return 0;
}
