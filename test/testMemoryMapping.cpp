#include <pona/stdio>
#include <pona/process>
#include <pona/thread>

namespace pona
{

const int mapLength = 256;

class CloneFactory: public ProcessFactory
{
public:
	CloneFactory(String path)
		: path_(path)
	{}
	
	int run()
	{
		File file(path_);
		file.open(File::Read);
		print("(clone) waiting for read access\n");
		FileLock lock(&file, FileLock::Read);
		lock.acquire();
		print("(clone) granted read access\n");
		{
			MemoryMapping mapping(&file, 0, mapLength);
			print("(clone) reads: \"%%\"\n", (const char*)mapping.start());
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
	File file("testMemoryMapping.XXXXXX");
	file.createUnique();
	file.unlinkOnExit();
	print("(parent) file.path() = \"%%\"\n", file.path());
	
	file.open(File::Read|File::Write);
	file.truncate(mapLength);
	print("(parent) acquiring write lock... \n");
	FileLock lock(&file, FileLock::Write);
	lock.acquire();
	
	print("(print) mapping file and writing message... \n");
	{
		MemoryMapping mapping(&file, 0, mapLength);
		String message = "Hello, clone!";
		for (int i = 0; i < message->length(); ++i)
			((char*)mapping.start())[i] = message->get(i);
	}
	
	print("(parent) cloning myself... \n");
	Ref<CloneFactory, Owner> factory = new CloneFactory(file.path());
	Ref<Process, Owner> fork = factory->produce();
	
	//print("(parent) sleeping 2 seconds... \n");
	//Thread::sleep(2);
	
	print("(parent) releasing write lock... \n");
	lock.release();
	
	int ret = fork->wait();
	print("(parent) clone terminated, ret = %%\n", ret);
	
	return 0;
}

} // namespace pona

int main()
{
	return pona::main();
}
