#include <pona/Core.hpp>

namespace pona
{

class MyChannel
{
public:
	MyChannel()
		: empty_(1),
		  full_(0)
	{}

	void put(int value)
	{
		empty_.acquire();
		value_ = value;
		full_.release();
	}

	int get()
	{
		full_.acquire();
		int value = value_;
		empty_.release();
		return value;
	}

private:
	Semaphore empty_;
	Semaphore full_;
	int value_;
};

class MyThread: public Thread
{
public:
	enum Role { Producer, Consumer };

	MyThread(int role, MyChannel* channel)
		: role_(role),
		  channel_(channel)
	{}
	
	int run()
	{
		if (role_ == Producer)
		{
			for (int i = 0; i < 10; ++i)
			{
				output()->writeLine(format("producer: i = %%") % i);
				channel_->put(i);
			}
		}
		else // if (role == Consumer)
		{
			int k = 0;
			while (k != 9)
			{
				k = channel_->get();
				output()->writeLine(format("consumer: k = %%") % k);
			}
		}

		return 0;
	}
	
private:
	int role_;
	MyChannel* channel_;
};

int main()
{
	MyChannel channel;
	MyThread t1(MyThread::Producer, &channel);
	MyThread t2(MyThread::Consumer, &channel);
	TimeStamp dt = getTime();
	t1.start();
	t2.start();
	t1.wait();
	t2.wait();
	dt = getTime() - dt;
	output()->write(format("\ndt = %% usec\n\n") % dt.microSeconds());

	return 0;
}

} // namespace pona

int main()
{
	return pona::main();
}
