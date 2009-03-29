#include <pona/stdio>
#include <pona/context>

namespace pona
{

void printStatus(Ref<File, Owner> file)
{
	print("file->path() = \"%%\"\n", file->path());
	print("file->exists() = %%\n", file->exists());
	print("file->access(File::Read) = %%\n", file->access(File::Read));
	print("file->access(File::Write) = %%\n", file->access(File::Write));
	print("file->access(File::Execute) = %%\n", file->access(File::Execute));
	Ref<FileStatus, Owner> status = file->status();
	if (status) {
		print("status->type() = %oct%\n", status->type());
		print("status->mode() = %oct%\n", status->mode());
		print("status->size() = %%\n", status->size());
		print("status->ownerId() = %%\n", status->ownerId());
		print("status->groupId() = %%\n", status->groupId());
		print("User(status->ownerId()).name() = %%\n", User(status->ownerId()).name());
		print("Group(status->groupId()).name() = %%\n", Group(status->groupId()).name());
	}
	print("\n");
}

int main(int argc, char** argv)
{
	printStatus(new File("/usr/include"));
	printStatus(new File(argv[0]));
	printStatus(new File("hmpf.xyz"));
	printStatus(rawInput());
	return 0;
}

} // namespace pona

int main(int argc, char** argv)
{
	return pona::main(argc, argv);
}
