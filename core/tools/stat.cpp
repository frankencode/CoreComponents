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
			"  -path     file path\n"
			"  -name     file name\n"
			"  -type     file type\n"
			"  -mode     file mode\n"
			"  -size     file size\n"
			"  -du       disk usage\n"
			"  -owner    owner\n"
			"  -group    group\n"
			"  -ta       access time\n"
			"  -tm       modified time\n"
			"  -tc       changed time\n"
			"  -ino      inode number\n"
			"  -links    number of hard links\n"
			"  -human    optimize for readability\n",
			String(argv[0])->fileName()
		);
		return 0;
	}

	auto listOfFiles = config->arguments();

	bool defaults = (config->options()->length() == 0);

	bool humanOption        = config->contains("human") || defaults;
	bool pathOption         = config->contains("path");
	bool nameOption         = config->contains("name")  || defaults;
	bool typeOption         = config->contains("type");
	bool modeOption         = config->contains("mode");
	bool sizeOption         = config->contains("size")  || defaults;
	bool diskUsageOption    = config->contains("du");
	bool ownerOption        = config->contains("owner") || defaults;
	bool groupOption        = config->contains("group") || defaults;
	bool accessTimeOption   = config->contains("ta");
	bool modifiedTimeOption = config->contains("tm")    || defaults;
	bool changedTimeOption  = config->contains("tc");
	bool inodeNumberOption  = config->contains("inode");
	bool linksOption        = config->contains("links") || defaults;

	Ref< Source<String> > files;
	if (rawInput()->isTeletype())
		files = listOfFiles;
	else
		files = input();

	for (String file; files->read(&file);)
	{
		auto status = FileStatus::read(file);

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
