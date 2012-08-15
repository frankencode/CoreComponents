#include <ftl/stdio>
#include <ftl/threads>
#include <ftl/utils>

namespace ftl
{

// typedef Rondezvous<int> MyChannel;
// typedef Conveyor<int> MyChannel;
typedef Channel<int> MyChannel;

class Consumer: public Thread
{
public:
	Consumer(int id, Ref<MyChannel> channel, int amount)
		: id_(id),
		  channel_(channel),
		  amount_(amount)
	{}

private:
	void run()
	{
		while (amount_ > 0) {
			int x = channel_->pop();
			print("consumer %%: consuming %%\n", id_, x);
			--amount_;
		}
	}

	int id_;
	Ref<MyChannel, Owner> channel_;
	int amount_;
};

class Producer: public Thread
{
public:
	Producer(int id, Ref<MyChannel> channel, int amount)
		: id_(id),
		  channel_(channel),
		  amount_(amount),
		  random_(Random::newInstance(amount))
	{}

	void run()
	{
		while (amount_ > 0) {
			int x = random_->get();
			print("producer %%: producing %%\n", id_, x);
			channel_->push(x);
			--amount_;
		}
	}

private:
	int id_;
	Ref<MyChannel, Owner> channel_;
	int amount_;
	Ref<Random, Owner> random_;
};

int main()
{
	Ref<MyChannel, Owner> channel = MyChannel::newInstance();

	Ref<Producer, Owner> p1 = new Producer(1, channel, 8);
	// Ref<Producer, Owner> p2 = new Producer(2, channel, 12);
	Ref<Consumer, Owner> c1 = new Consumer(1, channel, 8);
	// Ref<Consumer, Owner> c2 = new Consumer(2, channel, 16);

	Time dt = Time::now();
	c1->start();
	p1->start();
	//c2->start();
	//p2->start();
	c1->wait();
	//c2->wait();
	p1->wait();
	//p2->wait();
	dt = Time::now() - dt;

	print("\ndt = %% us\n\n", dt.us());

	return 0;
}

} // namespace ftl

int main()
{
	return ftl::main();
}
