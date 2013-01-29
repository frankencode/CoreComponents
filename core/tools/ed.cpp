#include <ftl/PrintDebug.hpp>
#include <ftl/Config.hpp>
#include <ftl/Pattern.hpp>
#include <ftl/File.hpp>

int main(int argc, char **argv)
{
	using namespace ftl;

	hook<Config> config = Config::create();
	config->read(argc, argv);
	if (config->contains("h") || config->contains("help")) {
		print(
			"Usage: %% [OPTION]... < [COMMANDS]\n"
			"Display and edit text sections. Commands of the following form are\n"
			"read from standard input:\n"
			"<path>[:<line>][:<start offset>:<end offset>][:<command>[=value]]\n"
			"\n"
			"Commands:\n"
			"  view       view text section, highlight match\n"
			"  print      print the matching text\n"
			"  replace    replace text (value is the replacement text)\n"
			"  paste      paste file (value is the file path)\n"
			"  delete     delete text\n"
			"\n"
			"Options:\n"
			"  -replace   replacement text\n"
			"  -paste     replacement text from file\n"
			"  -delete    delete text\n",
			string(argv[0])->fileName()
		);
		return 0;
	}

	string globalCommand, globalValue;

	if (config->contains("print")) {
		globalCommand = "print";
	}
	else if (config->contains("replace")) {
		globalCommand = "replace";
		globalValue = config->value("replace");
	}
	else if (config->contains("paste")) {
		globalCommand = "replace";
		globalValue = File::open(config->value("paste"))->map();
	}
	else if (config->contains("delete")) {
		globalCommand = "replace";
	}

	Pattern commandSyntax(
		"(?path:[^:]{~})"
		"(:(?ln:[0..9]{1,})){0,1}"
		"(:(?i0:[0..9]{1,})..(?i1:[0..9]{1,})){0,1}"
		"(:(?command:([a..z]|[A..Z]|_){1,}(=(?value:#{1,})){0,1})){0,1}"
	);

	for (string line; input()->read(&line);)
	{
		hook<SyntaxState> state = commandSyntax->newState();
		if (!commandSyntax->match(line, 0, state)) {
			printTo(error(), "Syntax error: \"%%\"\n", line);
			return 1;
		}

		string path = line->copy(state->capture("path"));
		int ln = -1; {
			Range *range = state->capture("ln");
			if (range->valid()) ln = line->copy(range)->toInt();
		}
		int i0 = 0, i1 = -1; {
			Range *range = state->capture("i0");
			if (range->valid()) {
				i0 = line->copy(range)->toInt();
				i1 = line->copy(state->capture("i1"))->toInt();
			}
		}

		string command = globalCommand; if (command == "") {
			Range *range = state->capture("command");
			if (range->valid()) command = line->copy(range);
			else command = "view";
		}
		string value = globalValue; if (value == "") {
			Range *range = state->capture("value");
			if (range->valid()) value = line->copy(range);
		}

		if (command == "paste") {
			command = "replace";
			value = File::open(value)->map();
		}
		else if (command == "delete") {
			command = "replace";
			value = "";
		}

		string text = File::open(path)->map();

		if (i1 < 0) {
			if (ln > 0) {
				if (!text->linePosToOffset(ln, 1, &i0)) {
					printTo(error(), "Invalid line number: %%\n", ln);
					return 1;
				}
				for (i1 = i0; i1 < text->length(); ++i1)
					if (text->at(i1) == '\n') break;
			}
			else {
				i0 = 0;
				i1 = text->length();
			}
		}
		else if (!(0 <= i0 && i0 <= i1 && i1 <= text->length())) {
			printTo(error(), "Invalid byte range: %%..%%\n", i0, i1);
			return 1;
		}
		if (ln < 0)
			text->offsetToLinePos(i0, &ln);

		if (command == "view") {
			print("%%:", path);
			if (i0 == i1) {
				print("%%: %%\n", ln, text->copy(i0, text->find("\n", i0)));
				continue;
			}

			bool multiline = text->copy(i0, i1)->contains('\n');
			if (multiline) output()->write("\n");

			int j0 = i0;
			for (;j0 > 0; --j0)
				if (text->at(j0 - 1) == '\n') break;

			for (int j1 = j0; j0 < i1; j0 = j1) {
				for (;j1 < text->length(); ++j1)
					if (text->at(j1) == '\n') break;
				format line;
				line << ln << ": ";
				int k0 = j0, k1 = j1;
				if (j0 <= i0 && i0 < j1) k0 = i0;
				if (j0 < i1 && i1 < j1) k1 = i1;
				if (j0 < k0) line << text->copy(j0, k0);
				if (k0 < k1) line << "\033[7m" << text->copy(k0, k1) << "\033[m";
				if (k1 < j1) line << text->copy(k1, j1);
				line << "\n";
				output()->write(line->join());
				++ln;
				++j1;
			}
		}
		else if (command == "print") {
			print("%%: %%\n", path, text->copy(i0, i1));
		}
		else if (command == "replace") {
			string newText = text->copy(0, i0) + value + text->copy(i1, text->length());
			hook<File> file = File::open(path, File::Write);
			file->truncate(0);
			file->write(newText);
		}
	}

	return 0;
}
