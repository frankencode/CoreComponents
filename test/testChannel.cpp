#include <pona/Core.hpp>

namespace pona
{

typedef Channel<int> MyChannel;

class MyThread: public Thread
{
public:
	enum Role { consumer, producer };
	
	MyThread(int role, Ref<MyChannel> channel)
		: role_(role),
		  channel_(channel)
	{}
	
	int run()
	{
		const int n = 256;
		
		if (role_ == producer)
		{
			for (int i = 0; i < n; ++i)
			{
				output()->writeLine(format("producer: i = %%") % i);
				channel_->pushBack(i);
			}
		}
		else // if (role_ == consumer)
		{
			int k = 0;
			while (k != n-1)
			{
				k = channel_->popFront();
				output()->writeLine(format("consumer: k = %%") % k);
			}
		}
		return 0;
	}
	
private:
	int role_;
	Ref<MyChannel, Owner> channel_;
};

int main()
{
	Ref<MyChannel, Owner> channel = new MyChannel(16);
	new Bouncer(channel);
	
	Ref<MyThread, Owner> t1 = new MyThread(MyThread::producer, channel);
	Ref<MyThread, Owner> t2 = new MyThread(MyThread::consumer, channel);
	
	TimeStamp dt = getTime();
	t1->start();
	t2->start();
	t1->wait();
	t2->wait();
	dt = getTime() - dt;
	output()->write(format("\ndt = %% usec\n\n") % dt.microSeconds());
	
#ifdef PONA_WINDOWS
	output()->write("\nPress <RETURN> to continue...\n");
	input()->readLine();
#endif
	return 0;
}

} // namespace pona

int main()
{
	return pona::main();
}

