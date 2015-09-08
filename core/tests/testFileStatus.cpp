/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio>
#include <flux/check>
#include <flux/File>
#include <flux/User>
#include <flux/Group>

using namespace flux;

int main(int argc, char **argv)
{
    String path = argv[0];
    fout("path = \"%%\"\n") << path;
    fout("File::exists(\"%%\") = %%\n") << path << File::exists(path);
    fout("File::access(\"%%\", File::Read) = %%\n") << path << File::access(path, File::Readable);
    fout("File::access(\"%%\", File::Write) = %%\n") << path << File::access(path, File::Writeable);
    fout("File::access(\"%%\", File::Execute) = %%\n") << path << File::access(path, File::Executable);
    if (!File::access(path, File::Executable)) return 1;
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
    catch (std::exception &ex) {
        ferr() << ex.what() << nl;
        check(false);
    }
    return 0;
}
