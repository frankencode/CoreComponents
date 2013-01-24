#include <ftl/stdio>
#include <ftl/misc>

using namespace ftl;

int main() {
	String text =
		"Person {\n"
		"  name:  \"Hans Mustermann\"\n"
		"  age: 17\n"
		"  hobbies: [ \"Sky Diving\", \"Mountain Biking\", \"Poetry\" ]\n"
		"  picture: {\n"
		"    uri: \"http://www.hans-mustermann.de/photo.jpg\"\n"
		"    width: 400; height: 300\n"
		"  }\n"
		"  home: {\n"
		"    latitude: 12.34\n"
		"    longitude: 123.4\n"
		"  }\n"
		"  favouriteNumbers: [ 2 5 7 ]\n"
		"  \"Is a super hero?\": true\n"
		"}";
	print("%%\n", text);
	Ref<WireObject, Owner> object = wire()->parse(text);
	print("\n");
	print("className: %%\n", object->className());
	print("name: %%\n", object->value("name"));
	print("age: %%\n", object->value("age"));
	print("hobbies: ");
	VariantList *hobbies = cast<VariantList>(object->value("hobbies"));
	for (int i = 0; i < hobbies->length(); ++i) {
		print("%%", hobbies->get(i));
		if (i != hobbies->length() - 1)
			print(", ");
	}
	print("\n");
	print("picture:\n");
	WireObject *picture = cast<WireObject>(object->value("picture"));
	print("  uri: %%\n", picture->value("uri"));
	print("  width: %%\n", picture->value("width"));
	print("  height: %%\n", picture->value("height"));
	WireObject *home = cast<WireObject>(object->value("home"));
	print("home:\n");
	print("  latitude: %%\n", home->value("latitude"));
	print("  longitude: %%\n", home->value("longitude"));
	print("favouriteNumbers: ");
	VariantList *numbers = cast<VariantList>(object->value("favouriteNumbers"));
	for (int i = 0; i < numbers->length(); ++i) {
		print("%%", numbers->get(i));
		if (i != numbers->length() - 1)
			print(", ");
	}
	print("\n");
	print("Is a super hero?: %%\n", object->value("Is a super hero?"));
	return 0;
}
