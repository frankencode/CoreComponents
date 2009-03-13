#include <pona/stdio>
#include <pona/concurrent>
#include <pona/container>
#include <pona/misc>

namespace pona
{

typedef Channel<int> MyChannel;

class MyThread: public Thread
{
public:
	enum Role { Consumer, Producer };
	
	MyThread(int role, Ref<MyChannel> channel)
		: role_(role),
		  channel_(channel)
	{}
	
	int run()
	{
		const int n = 256;
		
		if (role_ == Producer)
		{
			for (int i = 0; i < n; ++i)
			{
				output()->writeLine(format("producer: i = %%") % i);
				channel_->pushBack(i);
			}
		}
		else // if (role_ == Consumer)
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
	
	Ref<MyThread, Owner> t1 = new MyThread(MyThread::Producer, channel);
	Ref<MyThread, Owner> t2 = new MyThread(MyThread::Consumer, channel);
	
	TimeStamp dt = getTime();
	t1->start();
	t2->start();
	t1->wait();
	t2->wait();
	dt = getTime() - dt;
	output()->write(format("\ndt = %% usec\n\n") % dt.microSeconds());
	
	return 0;
}

} // namespace pona

int main()
{
	return pona::main();
}

