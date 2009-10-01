#include <pona/stdio>
#include <pona/threads>
#include <pona/container>
#include <pona/misc>
#include <pona/time>

namespace pona
{

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
		  amount_(amount)
	{}
	
	void run()
	{
		while (amount_ > 0) {
			int x = random_.next();
			print("producer %%: producing %%\n", id_, x);
			channel_->push(x);
			--amount_;
		}
	}
	
private:
	int id_;
	Ref<MyChannel, Owner> channel_;
	int amount_;
	Random random_;
};

int main()
{
	Ref<MyChannel, Owner> channel = new MyChannel(16);
	
	Ref<Producer, Owner> p1 = new Producer(1, channel, 8);
	Ref<Producer, Owner> p2 = new Producer(2, channel, 12);
	Ref<Consumer, Owner> c1 = new Consumer(1, channel, 4);
	Ref<Consumer, Owner> c2 = new Consumer(2, channel, 16);
	
	Time dt = now();
	c1->start();
	p1->start();
	c2->start();
	p2->start();
	c1->wait();
	c2->wait();
	p1->wait();
	p2->wait();
	dt = now() - dt;
	
	print("\ndt = %% us\n\n", dt.us());
	
	return 0;
}

} // namespace pona

int main()
{
	return pona::main();
}
