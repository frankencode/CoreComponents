#include <cc/Thread>
#include <cc/Channel>
#include <cc/PriorityChannel>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "ProducerConsumer",
        []{
            const int n = 20;
            Channel<int> channel;

            Thread consumer {[&]{
                int y = 0;
                for (int x = 0; channel.popFront(&x); ++y)
                    CC_CHECK_EQUALS(x, y);
                CC_CHECK_EQUALS(y, n);
            }};

            Thread producer {[&]{
                for (int i = 0; i < n; ++i)
                    channel.emplaceBack(i);
                channel.close();
            }};

            consumer.start();
            producer.start();
            consumer.wait();
            producer.wait();
        }
    };


    TestCase {
        "PrioritizedProducerConsumer",
        []{
            const int n = 20;
            PriorityChannel<int> channel;

            Thread consumer {[&]{
                int y = 0;
                for (int x = 0; channel.read(&x); ++y)
                    CC_CHECK_EQUALS(x, y);
                CC_CHECK_EQUALS(y, n);
            }};

            Thread producer {[&]{
                for (int i = 0; i < n; ++i)
                    channel.write(i, i);
                channel.close();
            }};

            consumer.start();
            producer.start();
            consumer.wait();
            producer.wait();
        }
    };

    return TestSuite{argc, argv}.run();
}
