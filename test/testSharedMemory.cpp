#include <pona/Core.hpp>
#include <string.h>

namespace pona
{

int main(int argc, char** argv)
{
	const char* message = "Hellow, world!";
	int size = strlen(message) + 1;
	SharedMemory mem("/testSharedMemory_shm_mem", size);
	Semaphore avail("testSharedMemory_sem_avail");
	Semaphore commit("testSharedMemory_sem_commit");
	
	if (mem.firstInstance())
	{
		memcpy(mem.region(), message, size);
		avail.release();
		
		output()->write(
			"First instance:\n"
			"  Launch second instance of this program to complete the test.\n"
		);
		
		commit.acquire();
	}
	else
	{
		avail.acquire();
		
		const char* message2 = (const char*)mem.region();
		output()->write(
			format(
				"Second instance:\n"
				"  message = \"%%\"\n"
			) % message2
		);
		
		commit.release();
	}
	
	return 0;
}

} // namespace pona

int main(int argc, char** argv)
{
	return pona::main(argc, argv);
}
