#include <pona/threads>
#include <pona/stdio>

namespace pona
{

int main()
{
	{	// basic logic test
		
		class MyFiber: public Fiber
		{
		public:
			virtual void run() {
				print("(f) Hello, world!\n");
				yield();
				print("(f) Bye.\n");
			}
		};
		
		MyFiber fiber;
		fiber.resume();
		print("(c) Hello, echo!\n");
		fiber.resume();
		print("(c) Ack.\n");
	}
	
	{	// stack protection test
		
		class MyFiber: public Fiber
		{
		public:
			/* enable to perform stack overflow testing (see below)
			MyFiber()
				: Fiber(8192)
			{}*/
			virtual void run() {
				wormHole(0);
			}
			void wormHole(int depth) {
				int onStack = depth;
				print("(ahhh...) depth = %%, &onStack = 0x%hex%\n", depth, &onStack);
				wormHole(depth + 1);
			}
		};
		
		MyFiber fiber;
		
		print("\n");
		print("fiber.stack() = 0x%hex%\n", fiber.stack());
		print("fiber.stackSize() = %% = 0x%hex%\n", fiber.stackSize(), fiber.stackSize());
		print("fiber.guardSize() = %% = 0x%hex%\n", fiber.guardSize(), fiber.guardSize());
		print("Context::stackGrowsDownwards() = %%\n", Context::stackGrowsDownwards());
		
		// fiber.resume();
	}
	
	return 0;
}

} // namespace pona

int main()
{
	return pona::main();
}
