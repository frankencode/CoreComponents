#include <ftl/streams>
#include <ftl/remoting>

using namespace ftl;

int main() {
	String text =
		"{\n"
		"  \"Name\": \"Hans Mustermann\",\n"
		"  \"Age\": 17,\n"
		"  \"Hobbies\": [ \"Sky Diving\", \"Mountain Biking\", \"Poetry\" ],\n"
		"  \"Picture\": {\n"
		"    \"Uri\": \"http://www.hans-mustermann.de/photo.jpg\",\n"
		"    \"Width\": 400,\n"
		"    \"Height\": 300\n"
		"  },\n"
		"  \"Home\": {\n"
		"    \"Latitude\": 12.34,\n"
		"    \"Longitude\": 123.4\n"
		"  },\n"
		"  \"FavouriteNumbers\": [ 2, 5, 7 ]\n"
		"}";
	print("text = %%\n", text);
	Ref<JsonObject, Owner> object = json()->parse(text);
	print("\n");
	print("Name: %%\n", object->value("Name"));
	print("Age: %%\n", object->value("Age"));
	print("Hobbies: ");
	Ref<JsonArray> hobbies = object->value("Hobbies");
	for (int i = 0; i < hobbies->length(); ++i) {
		print("%%", hobbies->get(i));
		if (i != hobbies->length() - 1)
			print(", ");
	}
	print("\n");
	print("Picture:\n");
	Ref<JsonObject> picture = object->value("Picture");
	print("  Uri: %%\n", picture->value("Uri"));
	print("  Width: %%\n", picture->value("Width"));
	print("  Height: %%\n", picture->value("Height"));
	Ref<JsonObject> home = object->value("Home");
	print("Home:\n");
	print("  Latitude: %%\n", home->value("Latitude"));
	print("  Longitude: %%\n", home->value("Longitude"));
	print("Favourite Numbers: ");
	Ref<JsonArray> numbers = object->value("FavouriteNumbers");
	for (int i = 0; i < numbers->length(); ++i) {
		print("%%", numbers->get(i));
		if (i != numbers->length() - 1)
			print(", ");
	}
	print("\n");
	return 0;
}
