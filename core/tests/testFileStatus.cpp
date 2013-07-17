#include <fkit/stdio.h>
#include <fkit/check.h>
#include <fkit/User.h>
#include <fkit/Group.h>

using namespace fkit;

int main(int argc, char **argv)
{
	String path = argv[0];
	fout("path = \"%%\"\n") << path;
	fout("File::exists(\"%%\") = %%\n") << path << File::exists(path);
	fout("File::access(\"%%\", File::Read) = %%\n") << path << File::access(path, File::Read);
	fout("File::access(\"%%\", File::Write) = %%\n") << path << File::access(path, File::Write);
	fout("File::access(\"%%\", File::Execute) = %%\n") << path << File::access(path, File::Execute);
	if (!File::access(path, File::Execute)) return 1;
	try {
		if (File::exists(path)) {
			Ref<FileStatus> status = File::status(path);
			if (status) {
				fout("status->type() = %%\n") << oct(status->type());
				fout("status->mode() = %%\n") << oct(status->mode());
				fout("status->size() = %%\n") << status->size();
				fout("status->ownerId() = %%\n") << status->ownerId();
				fout("status->groupId() = %%\n") << status->groupId();
				fout("User(status->ownerId()).loginName() = %%\n") << User::lookup(status->ownerId())->loginName();
				try {
					fout("Group(status->groupId()).name() = %%\n") << Group::lookup(status->groupId())->name();
				}
				catch(...) {
					// we may not have enough rights on some systems
				}
			}
		}
		fout("\n");
	}
	catch (AnyException &ex) {
		ferr() << ex.what() << nl;
		check(false);
	}
	return 0;
}
