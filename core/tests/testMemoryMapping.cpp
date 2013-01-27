#include <ftl/stdio>
#include <ftl/process>
#include <ftl/threads>

namespace ftl
{

class CloneFactory: public ProcessFactory
{
public:
	static O<CloneFactory> create(String path) { return new CloneFactory(path); }

	int incarnate()
	{
		print("(clone) waiting for read access\n");
		auto file = File::open(path_, File::Read);
		auto lock = FileLock::create(file, FileLock::Read);
		Guard<FileLock> guard(lock);
		print("(clone) granted read access\n");
		print("(clone) reads: \"%%\"\n", file->map());
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
	auto file = File::temp();
	file->unlinkOnExit();
	print("(parent) file->path() = \"%%\"\n", file->path());

	print("(parent) acquiring write lock... \n");
	auto lock = FileLock::create(file, FileLock::Write);
	lock->acquire();

	print("(parent) writing message... \n");
	file->write("Hello, clone!");

	print("(parent) cloning myself... \n");
	auto factory = CloneFactory::create(file->path());
	auto fork = factory->produce();

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
