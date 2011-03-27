#include <ftl/streams>
#include <ftl/threads>
#include <ftl/utils>

namespace ftl
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
	void run()
	{
		for (int i = 0; i < 10; ++i) {
			print("producer: i = %%\n", i);
			channel_->put(i);
		}
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
	void run()
	{
		int k = 0;
		while (k != 9) {
			k = channel_->get();
			print("consumer: k = %%\n", k);
		}
	}
	
private:
	Ref<MyChannel> channel_;
};

int main()
{
	Ref<MyChannel, Owner> channel = new MyChannel;
	Producer producer(channel);
	Consumer consumer(channel);
	Time dt = Time::now();
	producer.start();
	consumer.start();
	producer.wait();
	consumer.wait();
	dt = Time::now() - dt;
	print("\ndt = %% us\n\n", dt.us());
	
	{
		ThreadFactory factory;
		print("default stack size = %%\n", int(factory.stackSize()));
		print("default guard size = %%\n", int(factory.guardSize()));
	}
	
	return 0;
}

} // namespace ftl

int main()
{
	return ftl::main();
}
