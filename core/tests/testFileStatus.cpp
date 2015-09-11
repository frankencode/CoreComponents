/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/testing/TestSuite>
#include <flux/stdio>
#include <flux/File>
#include <flux/User>
#include <flux/Group>

using namespace flux;
using namespace flux::testing;

class ProgramAccess: public TestCase
{
    void run()
    {
        String path = testSuite()->argv()[0];
        fout("path = \"%%\"\n") << path;
        fout("File::exists(\"%%\") = %%\n") << path << File::exists(path);
        fout("File::access(\"%%\", File::Read) = %%\n") << path << File::access(path, File::Readable);
        fout("File::access(\"%%\", File::Write) = %%\n") << path << File::access(path, File::Writeable);
        fout("File::access(\"%%\", File::Execute) = %%\n") << path << File::access(path, File::Executable);
        FLUX_VERIFY(File::access(path, File::Executable));
    }
};

class ProgramStatus: public TestCase
{
    void run()
    {
        String path = testSuite()->argv()[0];
        Ref<FileStatus> status = File::status(path);
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
};

int main(int argc, char **argv)
{
    FLUX_TESTSUITE_ADD(ProgramAccess);
    FLUX_TESTSUITE_ADD(ProgramStatus);

    return testSuite()->run(argc, argv);
}
