#include <pona/threads>
#include <pona/stdio>

namespace pona
{

int main()
{
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
	
	return 0;
}

} // namespace pona

int main()
{
	return pona::main();
}
