/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/File>
#include <cc/User>
#include <cc/Group>

using namespace cc;
using namespace cc::testing;

class ProgramAccess: public TestCase
{
    void run()
    {
        String path = TestSuite::instance()->execPath();
        fout("path = \"%%\"\n") << path;
        fout("File::exists(\"%%\") = %%\n") << path << File::exists(path);
        fout("File::access(\"%%\", ReadOk) = %%\n") << path << File::access(path, ReadOk);
        fout("File::access(\"%%\", WriteOk) = %%\n") << path << File::access(path, WriteOk);
        fout("File::access(\"%%\", ExecuteOk) = %%\n") << path << File::access(path, ExecuteOk);
        CC_VERIFY(File::access(path, ExecuteOk));
    }
};

class ProgramStatus: public TestCase
{
    void run()
    {
        String path = TestSuite::instance()->execPath();
        Ref<FileStatus> status = FileStatus::read(path);
        fout("status->type() = %%\n") << oct(int(status->type()));
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
    CC_TESTSUITE_ADD(ProgramAccess);
    CC_TESTSUITE_ADD(ProgramStatus);

    return TestSuite::instance()->run(argc, argv);
}