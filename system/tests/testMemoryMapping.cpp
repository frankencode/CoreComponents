#include <ftl/stdio>
#include <ftl/process>
#include <ftl/threads>

namespace ftl
{

const int mapLength = 256;

class CloneFactory: public ProcessFactory
{
public:
	CloneFactory(String path)
		: path_(path)
	{}

	int incarnate()
	{
		Ref<File, Owner> file = File::newInstance(path_);
		file->open(File::Read);
		print("(clone) waiting for read access\n");
		FileLock lock(file, FileLock::Read);
		lock.acquire();
		print("(clone) granted read access\n");
		{
			MemoryMapping mapping(file, 0, mapLength);
			print("(clone) reads: \"%%\"\n", reinterpret_cast<const char*>(mapping.start()));
		}
		lock.release();
		return 7;
	}

private:
	String path_;
};

int main()
{
	Process::cd("/tmp");
	Ref<File, Owner> file = File::newInstance("testMemoryMapping.XXXXXX");
	file->createUnique();
	file->unlinkOnExit();
	print("(parent) file.path() = \"%%\"\n", file->path());

	file->open(File::Read|File::Write);
	file->truncate(mapLength);
	print("(parent) acquiring write lock... \n");
	FileLock lock(file, FileLock::Write);
	lock.acquire();

	print("(parent) mapping file and writing message... \n");
	{
		MemoryMapping mapping(file, 0, mapLength);
		String message = "Hello, clone!";
		mem::cpy(reinterpret_cast<char*>(mapping.start()), message->data(), message->size());
	}

	print("(parent) cloning myself... \n");
	Ref<CloneFactory, Owner> factory = new CloneFactory(file->path());
	Ref<Process, Owner> fork = factory->produce();

	//print("(parent) sleeping 2 seconds... \n");
	//Thread::sleep(2);

	print("(parent) releasing write lock... \n");
	lock.release();

	int ret = fork->wait();
	print("(parent) clone terminated, ret = %%\n", ret);

	return 0;
}

} // namespace ftl

int main()
{
	return ftl::main();
}
