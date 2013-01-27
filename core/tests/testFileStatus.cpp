#include <ftl/stdio>
#include <ftl/process>

namespace ftl
{

void printStatus(String path)
{
	print("path = \"%%\"\n", path);
	print("File::exists(\"%%\") = %%\n", path, File::exists(path));
	print("File::access(\"%%\", File::Read) = %%\n", path, File::access(path, File::Read));
	print("File::access(\"%%\", File::Write) = %%\n", path, File::access(path, File::Write));
	print("File::access(\"%%\", File::Execute) = %%\n", path, File::access(path, File::Execute));
	if (File::exists(path)) {
		O<FileStatus> status = File::status(path);
		if (status) {
			print("status->type() = %oct%\n", status->type());
			print("status->mode() = %oct%\n", status->mode());
			print("status->size() = %%\n", status->size());
			print("status->ownerId() = %%\n", status->ownerId());
			print("status->groupId() = %%\n", status->groupId());
			print("User(status->ownerId()).loginName() = %%\n", User::lookup(status->ownerId())->loginName());
			try {
				print("Group(status->groupId()).name() = %%\n", Group::lookup(status->groupId())->name());
			}
			catch(...) {
				// we may not have enough rights on some systems
			}
		}
	}
	print("\n");
}

int main(int argc, char **argv)
{
	printStatus(argv[0]);
	printStatus("hmpf.xyz");
	// printStatus(rawInput());
	printStatus("/usr/include");
	return 0;
}

} // namespace ftl

int main(int argc, char **argv)
{
	return ftl::main(argc, argv);
}
