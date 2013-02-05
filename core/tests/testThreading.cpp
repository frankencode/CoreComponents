#include <ftl/PrintDebug.hpp>
#include <ftl/Semaphore.hpp>
#include <ftl/System.hpp>
#include <ftl/ThreadFactory.hpp>
#include <pthread.h>

namespace ftl
{

class MyChannel: public Instance
{
public:
	static hook<MyChannel> create() {
		return new MyChannel;
	}

	void put(int value)
	{
		empty_->acquire();
		value_ = value;
		full_->release();
	}

	int get()
	{
		full_->acquire();
		int value = value_;
		empty_->release();
		return value;
	}

private:
	MyChannel()
		: empty_(Semaphore::create(1)),
		  full_(Semaphore::create(0))
	{}

	hook<Semaphore> empty_;
	hook<Semaphore> full_;
	int value_;
};

class Producer: public Thread
{
public:
	static hook<Producer> create(MyChannel *channel) {
		return new Producer(channel);
	}

private:
	Producer(MyChannel *channel)
		: channel_(channel)
	{}

	void run()
	{
		for (int i = 0; i < 10; ++i) {
			print("producer: i = %%\n", i);
			channel_->put(i);
		}
	}

private:
	MyChannel *channel_;
};

class Consumer: public Thread
{
public:
	static hook<Consumer> create(MyChannel *channel) {
		return new Consumer(channel);
	}

private:
	Consumer(MyChannel *channel)
		: channel_(channel)
	{}

	void run()
	{
		int k = 0;
		while (k != 9) {
			k = channel_->get();
			print("consumer: k = %%\n", k);
		}
	}

private:
	MyChannel *channel_;
};

int main()
{
	hook<MyChannel> channel = MyChannel::create();
	hook<Producer> producer = Producer::create(channel);
	hook<Consumer> consumer = Consumer::create(channel);
	Time dt = Time::now();
	producer->start();
	consumer->start();
	producer->wait();
	consumer->wait();
	dt = Time::now() - dt;
	print("\ndt = %% us\n\n", dt.us());

	hook<ThreadFactory> factory = ThreadFactory::create();
	print("default stack size = %% bytes = %% MB\n", int(factory->stackSize()), double(factory->stackSize()) / 1024. / 1024.);
	print("default guard size = %%\n", int(factory->guardSize()));
	print("\n");

	print("System::concurrency() = %%\n", System::concurrency());
	#ifdef PTHREAD_KEYS_MAX
	print("PTHREAD_KEYS_MAX = %%\n", PTHREAD_KEYS_MAX);
	#endif

	return 0;
}

} // namespace ftl

int main()
{
	return ftl::main();
}
