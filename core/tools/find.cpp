#include <ftl/PrintDebug.hpp>
#include <ftl/Config.hpp>
#include <ftl/Pattern.hpp>
#include <ftl/DirWalker.hpp>
#include <ftl/File.hpp>

int main(int argc, char **argv)
{
	using namespace ftl;

	auto config = Config::create();
	config->read(argc, argv);
	if (config->contains("h") || config->contains("help")) {
		print(
			"Usage: %% [OPTION]... [DIR]...\n"
			"Search in directories by file name, type or content.\n"
			"\n"
			"Options:\n"
			"  -name    name pattern\n"
			"  -type    type pattern\n"
			"             r .. regular file\n"
			"             d .. directory\n"
			"             l .. symbolic link\n"
			"             c .. character device\n"
			"             b .. block device\n"
			"             f .. fifo\n"
			"             s .. socket\n"
			"  -text    content pattern\n"
			"  -depth   maximum search depth\n",
			String(argv[0])->fileName()
		);
		return 0;
	}

	Pattern namePattern;
	Pattern typePattern;
	Pattern textPattern;
	int maxDepth = config->value("depth", -1);

	String h;
	if (config->lookup("name", &h)) namePattern = h;
	if (config->lookup("type", &h)) typePattern = h;
	if (config->lookup("text", &h)) textPattern = h;

	StringList *dirPaths = config->arguments();
	if (dirPaths->length() == 0) dirPaths->append(".");

	for (int i = 0; i < dirPaths->length(); ++i) {
		String dirPath = dirPaths->at(i)->canonicalPath();
		auto dirWalker = DirWalker::open(dirPath);
		dirWalker->setMaxDepth(maxDepth);
		String path;
		while (dirWalker->read(&path)) {
			if (namePattern) {
				if (!namePattern->match(path->fileName())) continue;
			}
			if (typePattern) {
				int type = FileStatus::read(path, false)->type();
				bool shortMode = (typePattern->matchLength() == 1);
				String typeString;
				if (type == File::Regular)          typeString = shortMode ? "r" : "regular file";
				else if (type == File::Directory)   typeString = shortMode ? "d" : "directory";
				else if (type == File::Link)        typeString = shortMode ? "l" : "link";
				else if (type == File::CharDevice)  typeString = shortMode ? "c" : "character device";
				else if (type == File::BlockDevice) typeString = shortMode ? "b" : "block device";
				else if (type == File::Fifo)        typeString = shortMode ? "f" : "fifo";
				else if (type == File::Socket)      typeString = shortMode ? "s" : "socket";
				if (!typePattern->find(typeString)) continue;
			}
			if (textPattern) {
				if (FileStatus::read(path)->type() != File::Regular)
					continue;
				Ref<File, Owner> file = File::tryOpen(path);
				if (!file) {
					printTo(error(), "Failed to open %%\n", path);
					continue;
				}
				String text = file->map();
				int ln = 1;
				for (int i = 0; i < text->length();) {
					Ref<Token, Owner> token = textPattern->find(text, i);
					if (!token) break;
					for (;i < token->i0(); ++i)
						if (text->at(i) == '\n') ++ln;
					print("%%:%%:%%..%%\n", path, ln, token->i0(), token->i1());
					for (;i < token->i1(); ++i)
						if (text->at(i) == '\n') ++ln;
					if (token->i0() == token->i1()) ++i;
				}
				continue;
			}
			output()->writeLine(path);
		}
	}

	return 0;
}
