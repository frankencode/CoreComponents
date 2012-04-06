#include <ftl/stdio>
#include <ftl/Wire.hpp>

using namespace ftl;

void print(Ref<Node> node, int depth = 0)
{
	if (node->type() == Node::AtomType) {
		print("\"%%\"", node->text());
	}
	else if (node->type() == Node::ObjectType) {
		Ref<ObjectNode> object = node;
		if (object->type() != "") print("%% ", object->type());
		print("{\n");
		for (int i = 0, n = object->memberCount(); i < n; ++i) {
			String memberName = object->memberName(i);
			print("%%%%: ", String(2 * (depth + 1), ' '), memberName);
			print(object->member(memberName), depth + 1);
			print("\n");
		}
		print("%%}", String(2 * depth, ' '));
		if (depth == 0) print("\n");
	}
	else if (node->type() == Node::ArrayType) {
		Ref<ArrayNode> array = node;
		print("[ ");
		for (int i = 0, n = array->itemCount(); i < n; ++i) {
			print(array->item(i));
			if (i != n - 1) print(", ");
		}
		print(" ]");
	}
}

int main() {
	String text =
		"Person {\n"
		"  age: 1\n"
		"  name: Hans Mustermann\n"
		"  hobbies: [ sky diving, mountain biking, poetry ]\n"
		"  picture: Image {\n"
		"    uri: http://www.hans-mustermann.de/photo.jpg\n"
		"    width: 400 /* hmmm*/\n"
		"    height: 300 // well\n"
		"  }\n"
		"  home: {\n"
		"    latitude: 12.34\n"
		"    longitude: 123.4\n"
		"  }\n"
		"  favouriteNumbers: [ 2/*!*/+3, /*!*/5, 7 ]\n"
		"  /*depends on x*/ z:\n"
		"    var h = x + 1 // helper variable\n"
		"    return h\n"
		"  x: 1; y: x\n"
		"}\n";
	print(text);
	Ref<Node, Owner> node = wire()->parse(text);
	print(node);
	return 0;
}
