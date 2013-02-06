#include <ftl/PrintDebug.hpp>
#include <ftl/ProcessFactory.hpp>
#include <ftl/FileLock.hpp>
#include <ftl/Thread.hpp>

namespace ftl
{

class CloneFactory: public ProcessFactory
{
public:
	static Ref<CloneFactory> create(string path) { return new CloneFactory(path); }

	int incarnate()
	{
		print("(clone) waiting for read access\n");
		Ref<File> file = File::open(path_, File::Read);
		Ref<FileLock> lock = FileLock::create(file, FileLock::Read);
		Guard<FileLock> guard(lock);
		print("(clone) granted read access\n");
		print("(clone) reads: \"%%\"\n", file->map());
		return 7;
	}

private:
	CloneFactory(string path)
		: path_(path)
	{}

	string path_;
};

int main()
{
	Ref<File> file = File::temp();
	file->unlinkOnExit();
	print("(parent) file->path() = \"%%\"\n", file->path());

	print("(parent) acquiring write lock... \n");
	Ref<FileLock> lock = FileLock::create(file, FileLock::Write);
	lock->acquire();

	print("(parent) writing message... \n");
	file->write("Hello, clone!");

	print("(parent) cloning myself... \n");
	Ref<ProcessFactory> factory = CloneFactory::create(file->path());
	Ref<Process> fork = factory->produce();

	print("(parent) sleeping 2 seconds... \n");
	Thread::sleep(2);

	print("(parent) releasing write lock... \n");
	lock->release();

	int ret = fork->wait();
	print("(parent) clone terminated, ret = %%\n", ret);

	return 0;
}

} // namespace ftl

int main()
{
	return ftl::main();
}
