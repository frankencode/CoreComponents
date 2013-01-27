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
	static O<Consumer> create(int id, MyChannel *channel, int amount) { return new Consumer(id, channel, amount); }

private:
	Consumer(int id, MyChannel *channel, int amount)
		: id_(id),
		  channel_(channel),
		  amount_(amount)
	{}

	void run()
	{
		while (amount_ > 0) {
			int x = channel_->pop();
			print("consumer %%: consuming %%\n", id_, x);
			--amount_;
		}
	}

	int id_;
	O<MyChannel> channel_;
	int amount_;
};

class Producer: public Thread
{
public:
	static O<Producer> create(int id, MyChannel *channel, int amount) { return new Producer(id, channel, amount); }

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
	Producer(int id, MyChannel *channel, int amount)
		: id_(id),
		  channel_(channel),
		  amount_(amount),
		  random_(Random::open(amount))
	{}

	int id_;
	O<MyChannel> channel_;
	int amount_;
	O<Random> random_;
};

int main()
{
	auto channel = MyChannel::create();
	auto p1 = Producer::create(1, channel, 8);
	auto c1 = Consumer::create(1, channel, 8);

	Time dt = Time::now();
	c1->start();
	p1->start();
	c1->wait();
	p1->wait();
	dt = Time::now() - dt;

	print("\ndt = %% us\n\n", dt.us());

	return 0;
}

} // namespace ftl

int main()
{
	return ftl::main();
}
