/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/SubProcess>
#include <cc/FileLock>
#include <cc/UnlinkGuard>
#include <cc/Guard>
#include <cc/Thread>

using namespace cc;
using namespace cc::testing;

class FileLockAndMap: public TestCase
{
    class Worker: public SubProcess::Worker
    {
    public:
        static Ref<Worker> create(String path) {
            return new Worker(path);
        }

    private:
        Worker(String path): path_(path)
        {}

        int run() override
        {
            fout("(clone) waiting for read access\n");
            Ref<File> file = File::open(path_, OpenMode::ReadOnly);
            Ref<FileLock> lock = FileLock::create(file, FileLock::ReadLock);
            Guard<FileLock> guard(lock);
            fout("(clone) granted read access\n");
            fout("(clone) reads: \"%%\"\n") << file->map();
            return 7;
        }

        String path_;
    };

    void run()
    {
        Ref<File> file = File::openTemp();
        UnlinkGuard unlinkGuard(file->path());

        fout("(parent) file->path() = \"%%\"\n") << file->path();

        fout("(parent) acquiring write lock... \n");
        Ref<FileLock> lock = FileLock::create(file, FileLock::WriteLock);
        lock->acquire();

        fout("(parent) writing message... \n");
        file->write("Hello, clone!");

        fout("(parent) forking a worker... \n");
        Ref<SubProcess> sub = SubProcess::open(Worker::create(file->path()));

        fout("(parent) sleeping 2 ms... \n");
        Thread::sleep(0.002);

        fout("(parent) releasing write lock... \n");
        lock->release();

        int ret = sub->wait();
        fout("(parent) clone terminated, ret = %%\n") << ret;

        CC_VERIFY(ret == 7);
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(FileLockAndMap);

    return TestSuite::instance()->run(argc, argv);
}
