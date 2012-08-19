#include <ftl/stdio>
#include <ftl/process>

namespace ftl
{

void printStatus(Ref<File, Owner> file)
{
	print("file->path() = \"%%\"\n", file->path());
	print("file->exists() = %%\n", file->exists());
	print("file->access(File::Read) = %%\n", file->access(File::Read));
	print("file->access(File::Write) = %%\n", file->access(File::Write));
	print("file->access(File::Execute) = %%\n", file->access(File::Execute));
	if (file->exists()) {
		Ref<FileStatus, Owner> status = FileStatus::newInstance(file->path());
		if (status) {
			print("status->type() = %oct%\n", status->type());
			print("status->mode() = %oct%\n", status->mode());
			print("status->size() = %%\n", status->size());
			print("status->ownerId() = %%\n", status->ownerId());
			print("status->groupId() = %%\n", status->groupId());
			print("User(status->ownerId()).loginName() = %%\n", User::newInstance(status->ownerId())->loginName());
			try {
				print("Group(status->groupId()).name() = %%\n", Group::newInstance(status->groupId())->name());
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
	printStatus(File::newInstance(argv[0]));
	printStatus(File::newInstance("hmpf.xyz"));
	printStatus(rawInput());
	printStatus(File::newInstance("/usr/include"));
	return 0;
}

} // namespace ftl

int main(int argc, char **argv)
{
	return ftl::main(argc, argv);
}
