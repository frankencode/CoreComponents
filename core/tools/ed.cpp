#include <ftl/PrintDebug.hpp>
#include <ftl/Config.hpp>
#include <ftl/Pattern.hpp>
#include <ftl/File.hpp>

int main(int argc, char **argv)
{
	using namespace ftl;

	auto config = Config::create(); // FIXME: need a Config::read() constructor
	config->read(argc, argv);
	if (config->contains("h") || config->contains("help")) {
		print("No help, yet.\n");
		return 0;
	}

	Pattern commandSyntax(
		"(?path:[^:]{~})"
		"(:(?ln:[0..9]{1,})){0,1}"
		"(:(?i0:[0..9]{1,})..(?i1:[0..9]{1,})){0,1}"
		"(:(?cmd:([a..z]|[A..Z]|_){1,})){0,1}"
	);

	for (String line; input()->read(&line);)
	{
		Ref<SyntaxState, Owner> state = commandSyntax->newState();
		if (!commandSyntax->match(line, 0, state)) {
			printTo(error(), "Syntax error: \"%%\"\n", line);
			return 1;
		}

		String path = line->copy(state->capture("path"));
		int ln = -1; {
			auto range = state->capture("ln");
			if (range->valid()) ln = line->copy(range)->toInt();
		}
		int i0 = 0, i1 = -1; {
			auto range = state->capture("i0");
			if (range->valid()) {
				i0 = line->copy(range)->toInt();
				i1 = line->copy(state->capture("i1"))->toInt();
			}
		}
		String cmd = "view"; {
			auto range = state->capture("cmd");
			if (range->valid()) cmd = line->copy(range);
		}

		// printTo(error(), "%%:%%:%%..%%:%%\n", path, ln, i0, i1, cmd);

		if (cmd == "view") {
			String text = File::open(path)->map();
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

			int j0 = i0;
			for (;j0 > 0; --j0)
				if (text->at(j0 - 1) == '\n') break;

			print("%%:", path);
			if (i0 == i1) {
				print("%%: %%\n", ln, text->copy(i0, text->find("\n", i0)));
				continue;
			}

			bool multiline = text->copy(i0, i1)->contains('\n');
			if (multiline) output()->write("\n");

			for (int j1 = j0; j0 < i1; j0 = j1) {
				for (;j1 < text->length(); ++j1)
					if (text->at(j1) == '\n') break;
				Format line;
				line << ln << ": ";
				int k0 = j0, k1 = j1;
				if (j0 < i0 && i1 <= i1)      { k0 = i0; k1 = i1; }
				else if (j0 <= i0 && i0 < j1) { k0 = i0; }
				else if (j0 < i1 && i1 <= j1) { k1 = i1; }
				if (j0 < k0)
					line << text->copy(j0, k0);
				if (k0 < k1)
					line << "\033[7m" << text->copy(k0, k1) << "\033[m";
				if (k1 < j1)
					line << text->copy(k1, j1);
				line << "\n";
				output()->write(line->join());
				++ln;
				++j1;
			}

			return 0;
		}
	}

	return 0;
}
