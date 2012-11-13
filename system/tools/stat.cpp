#include <ftl/stdio>
#include <ftl/process>
#include <ftl/container>
#include <ftl/utils>

using namespace ftl;

String fileTypeToString(int type)
{
	struct { int type; String name; } map[] = {
		{ File::Regular, "regular" },
		{ File::Directory, "directory" },
		{ File::CharDevice, "char_device" },
		{ File::BlockDevice, "block_device" },
		{ File::Fifo, "fifo" },
		{ File::Link, "symbolic_link" },
		{ File::Socket, "socket" }
	};
	const int mapSize = sizeof(map) / sizeof(map[0]);

	for (int i = 0; i < mapSize; ++i)
		if (type == map[i].type)
			return map[i].name;

	return Format("type_%%") << type;
}

String timeToString(Time time, bool human)
{
	auto d = Date::create(time);
	if (human) {
		return Format(
			"%4.:'0'%-%2.:'0'%-%2.:'0'% "
			"%2.:'0'%:%2.:'0'%"
		) << d->year() << d->month() << d->day()
		  << d->hour() << d->minute();
	}
	else {
		return d->toString();
	}
}

int main(int argc, char **argv)
{
	auto config = Config::create();
	config->read(argc, argv);

	if (config->contains("h") || config->contains("help")) {
		print(
			"Usage: %% [OPTION]... [FILE]...\n"
			"Display file status information.\n"
			"The list of files can also be given on standard input.\n"
			"\n"
			"Options:\n"
			"-p, --path      file path\n"
			"-n, --name      file name\n"
			"-t, --type      file type\n"
			"-x, --mode      file mode\n"
			"-s, --size      file size\n"
			"-d, --du        disk usage\n"
			"-o, --owner     owner\n"
			"-g, --group     group\n"
			"-a, --ta        access time\n"
			"-m, --tm        modified time\n"
			"-c, --tc        changed time\n"
			"-i, --ino       inode number\n"
			"-l, --links     number of hard links\n"
			"-u, --human     optimize for readability\n"
			"-h, --help      print help\n"
			"\n"
			"Report bugs to <frank@cyblogic.de>\n",
			String(argv[0])->fileName()
		);
		return 0;
	}

	auto listOfFiles = config->arguments();

	bool defaults = (config->options()->length() == 0);

	bool humanOption        = config->contains("u") || config->contains("human") || defaults;
	bool pathOption         = config->contains("p") || config->contains("path");
	bool nameOption         = config->contains("n") || config->contains("name")  || defaults;
	bool typeOption         = config->contains("t") || config->contains("type");
	bool modeOption         = config->contains("x") || config->contains("mode");
	bool sizeOption         = config->contains("s") || config->contains("size")  || defaults;
	bool diskUsageOption    = config->contains("d") || config->contains("du");
	bool ownerOption        = config->contains("o") || config->contains("owner") || defaults;
	bool groupOption        = config->contains("g") || config->contains("group") || defaults;
	bool accessTimeOption   = config->contains("a") || config->contains("ta");
	bool modifiedTimeOption = config->contains("m") || config->contains("tm")    || defaults;
	bool changedTimeOption  = config->contains("c") || config->contains("tc");
	bool inodeNumberOption  = config->contains("i") || config->contains("inode");
	bool linksOption        = config->contains("l") || config->contains("links") || defaults;

	Ref< Source<String> > files;
	if (rawInput()->isTeletype())
		files = listOfFiles;
	else
		files = input();

	for (String file; files->read(&file);)
	{
		auto status = FileStatus::create(file);

		Format line;

		if (nameOption)      line << status->path()->fileName();
		if (pathOption)      line << status->path();
		if (typeOption)      line << fileTypeToString(status->type());
		if (modeOption)      line << String(Format("%oct%") << status->mode());
		if (sizeOption)      line << status->size();
		if (diskUsageOption) line << status->sizeInBlocks() * status->sizeOfBlock();
		if (ownerOption)     line << User::lookup(status->ownerId())->name();

		if (groupOption) {
			try {
				line << Group::lookup(status->groupId())->name();
			}
			catch(...) {
				// we may not have enough rights on some systems
			}
		}

		if (accessTimeOption)   line << timeToString(status->lastAccess(), humanOption);
		if (modifiedTimeOption) line << timeToString(status->lastModified(), humanOption);
		if (changedTimeOption)  line << timeToString(status->lastChanged(), humanOption);
		if (inodeNumberOption)  line << status->inodeNumber();
		if (linksOption)        line << status->numberOfHardLinks();

		output()->writeLine(line->join("\t"));
	}

	return 0;
}
