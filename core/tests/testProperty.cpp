#include <ftl/stdio>
#include <ftl/Property.hpp>

namespace ftl
{

class Shape: public Instance
{
public:
	static hook<Shape> create() { return new Shape; }

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
		print("name = \"%%\"\n", name);
	}
};

class Observer: public Instance
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
		print("x = %%\n", value);
	}
	void onYChanged(int value) {
		print("y = %%\n", value);
	}

	hook<Shape> shape_;
};

int main()
{
	hook<Shape> shape = Shape::create();
	{
		Observer observer(shape);
		shape->name = "circle1";
		shape->x = 7;
		shape->y = 93;
		shape->y = 93;
	}
	shape->x = 8;
	return 0;
}

} // namespace ftl

int main()
{
	return ftl::main();
}
