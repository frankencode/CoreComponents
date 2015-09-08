/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio>
#include <flux/check>
#include <flux/ProcessFactory>
#include <flux/FileLock>
#include <flux/Guard>
#include <flux/Thread>

using namespace flux;

class CloneFactory: public ProcessFactory
{
public:
    static Ref<CloneFactory> create(String path) { return new CloneFactory(path); }

    int incarnate()
    {
        fout("(clone) waiting for read access\n");
        Ref<File> file = File::open(path_, File::ReadOnly);
        Ref<FileLock> lock = FileLock::create(file, FileLock::ReadLock);
        Guard<FileLock> guard(lock);
        fout("(clone) granted read access\n");
        fout("(clone) reads: \"%%\"\n") << file->map();
        return 7;
    }

private:
    CloneFactory(String path)
        : path_(path)
    {}

    String path_;
};

int main()
{
    Ref<File> file = File::temp();
    fout("(parent) file->path() = \"%%\"\n") << file->path();

    fout("(parent) acquiring write lock... \n");
    Ref<FileLock> lock = FileLock::create(file, FileLock::WriteLock);
    lock->acquire();

    fout("(parent) writing message... \n");
    file->write("Hello, clone!");

    fout("(parent) cloning myself... \n");
    Ref<ProcessFactory> factory = CloneFactory::create(file->path());
    Ref<Process> fork = factory->produce();

    fout("(parent) sleeping 2 ms... \n");
    Thread::sleep(0.002);

    fout("(parent) releasing write lock... \n");
    lock->release();

    int ret = fork->wait();
    fout("(parent) clone terminated, ret = %%\n") << ret;

    check(ret == 7);

    return 0;
}
