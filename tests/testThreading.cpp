#include <pona/stdio>
#include <pona/threads>
#include <pona/time>

namespace pona
{

class MyChannel: public Instance
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

class Producer: public Thread
{
public:
	Producer(Ref<MyChannel> channel)
		: channel_(channel)
	{}
	
private:
	int run()
	{
		for (int i = 0; i < 10; ++i) {
			print("producer: i = %%\n", i);
			channel_->put(i);
		}
		return 0;
	}
private:
	Ref<MyChannel> channel_;
};

class Consumer: public Thread
{
public:
	Consumer(Ref<MyChannel> channel)
		: channel_(channel)
	{}
	
private:
	int run()
	{
		int k = 0;
		while (k != 9) {
			k = channel_->get();
			print("consumer: k = %%\n", k);
		}
		return 0;
	}
private:
	Ref<MyChannel> channel_;
};

int main()
{
	Ref<MyChannel, Owner> channel = new MyChannel;
	Producer producer(channel);
	Consumer consumer(channel);
	Time dt = now();
	producer.start();
	consumer.start();
	producer.wait();
	consumer.wait();
	dt = now() - dt;
	print("\ndt = %% us\n\n", dt.us());
	
	return 0;
}

} // namespace pona

int main()
{
	return pona::main();
}
