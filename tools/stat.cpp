#include <ftl/streams>
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
		{ File::SymbolicLink, "symbolic_link" },
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
	Date d(time);
	if (human) {
		return Format(
			"%4.:'0'%-%2.:'0'%-%2.:'0'% "
			"%2.:'0'%:%2.:'0'%"
		) << d.year() << d.month() << d.day()
		  << d.hour() << d.minute();
	}
	else {
		return d.toString();
	}
}

int main(int argc, char** argv)
{
	CommandLine commandLine;
	commandLine.summary(
		"Display file status information.\n"
		"The list of files can also be given on standard input."
	);
	commandLine.details(
		"If no options are selected the default assumption is -mlogstnu.\n"
		"\n"
		"Report bugs to <frank@cyblogic.de>"
	);
	commandLine.entity("FILE");
	
	Ref<CommandOption> path  = commandLine.define('p', "path",  false, "File path");
	Ref<CommandOption> name  = commandLine.define('n', "name",  false, "File name");
	Ref<CommandOption> type  = commandLine.define('t', "type",  false, "File type");
	Ref<CommandOption> mode  = commandLine.define('x', "mode",  false, "File mode");
	Ref<CommandOption> size  = commandLine.define('s', "size",  false, "File size");
	Ref<CommandOption> du    = commandLine.define('d', "du",    false, "Disk usage");
	Ref<CommandOption> owner = commandLine.define('o', "owner", false, "Owner");
	Ref<CommandOption> group = commandLine.define('g', "group", false, "Group");
	Ref<CommandOption> ta    = commandLine.define('a', "ta",    false, "Latest access time");
	Ref<CommandOption> tm    = commandLine.define('m', "tm",    false, "Latest write access time");
	Ref<CommandOption> tc    = commandLine.define('c', "tc",    false, "Latest time when times changed");
	Ref<CommandOption> ino   = commandLine.define('i', "ino",   false, "Inode number");
	Ref<CommandOption> links = commandLine.define('l', "links", false, "Number of hard links");
	Ref<CommandOption> human = commandLine.define('u', "human", false, "Optimize readability for humans");
	Ref<CommandOption> help  = commandLine.define('h', "help",  false, "Print help");
	
	Ref<StringList, Owner> listOfFiles = commandLine.read(argc, argv);
	
	if (help->value()) {
		print(commandLine.helpText());
		return 0;
	}
	
	Ref<CommandLine::OptionList> options = commandLine.usedOptions();
	if (options->isEmpty() || (options->contains(human) && (options->length() == 1))) {
		options->clear();
		*options << mode << links << owner << group << size << tm << name << human;
	}
	
	Ref< Source<String> > files;
	if (rawInput()->isTeletype())
		files = listOfFiles;
	else
		files = input();
	
	for (String file; files->read(&file);) {
		FileStatus status(file);
		for (CommandLine::OptionList::Index i = options->first(); options->def(i); ++i) {
			Ref<CommandOption> option = options->at(i);
			if (option == name)
				print(Path(status.path()).fileName());
			if (option == path)
				print(status.path());
			if (option == type)
				print(fileTypeToString(status.type()));
			if (option == mode)
				print("%oct%", status.mode());
			if (option == size)
				print("%%", status.size());
			if (option == du)
				print("%%", status.sizeInBlocks() * status.sizeOfBlock());
			if (option == owner)
				print("%%", User(status.ownerId()).name());
			if (option == group) {
				try {
					print("%%", Group(status.groupId()).name());
				}
				catch(...) {
					// we may not have enough rights on some systems
				}
			}
			if (option == ta)
				print("%%", timeToString(status.lastAccess(), human->value()));
			if (option == tm)
				print("%%", timeToString(status.lastModified(), human->value()));
			if (option == tc)
				print("%%", timeToString(status.lastChanged(), human->value()));
			if (option == ino)
				print("%%", status.inodeNumber());
			if (option == links)
				print("%%", status.numberOfHardLinks());
			if (options->def(i + 1)) print("\t");
		}
		print("\n");
	}
	
	return 0;
}
