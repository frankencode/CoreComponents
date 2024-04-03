#include <cc/Heap>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "HeapSize2",
        []{
            MaxHeap<int> heap { Dim<>{2} };
            heap.push(1);
            CC_INSPECT(heap.top());
            heap.push(2);
            CC_INSPECT(heap.top());
            CC_INSPECT(heap.pop());
        }
    };

    TestCase {
        "HeapSort10",
        []{
            MinHeap<int> heap { Dim<>{10} };
            heap.push(6);
            CC_INSPECT(heap.top());
            heap.push(7);
            CC_INSPECT(heap.top());
            heap.push(5);
            CC_INSPECT(heap.top());
            heap.push(1);
            CC_INSPECT(heap.top());
            heap.push(3);
            CC_INSPECT(heap.top());
            heap.push(0);
            CC_INSPECT(heap.top());
            heap.push(2);
            CC_INSPECT(heap.top());
            heap.push(9);
            CC_INSPECT(heap.top());
            heap.push(4);
            CC_INSPECT(heap.top());
            heap.push(8);
            CC_INSPECT(heap.top());

            for (int i = 0; heap.fill() > 0; ++i) {
                CC_INSPECT(i);
                CC_VERIFY(heap.pop() == i);
            }
        }
    };

    return TestSuite{argc, argv}.run();
}
