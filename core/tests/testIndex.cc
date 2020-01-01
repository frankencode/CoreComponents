#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/debug>
#include <cc/index>

using namespace cc;
using namespace cc::index;
using namespace cc::testing;

class LeafTest: public TestCase
{
    void print(const Leaf<unsigned> *leaf) {
        Format f;
        for (unsigned i = 0; i < leaf->fill_; ++i) f << leaf->at(i);
        ferr() << "(" << f->join(", ") << ") " << nl;
    }

    void run() override
    {
        static constexpr unsigned n = Node::Capacity;
        Local<Leaf<unsigned>> leaf;
        for (unsigned i = 0; i < n; ++i) {
            leaf->push(i, i);
            print(leaf);
            CC_VERIFY(leaf->at(i) == i);
            CC_VERIFY(leaf->fill_ == i + 1);
        }
        for (unsigned i = 0; i < n; ++i) {
            CC_VERIFY(leaf->at(0) == i);
            CC_VERIFY(leaf->fill_ == n - i);
            leaf->pop(0);
            print(leaf);
        }
        for (unsigned i = 0; i < n; ++i) {
            leaf->push(0, i);
            print(leaf);
        }
        for (unsigned i = 0; i < n; ++i) {
            CC_VERIFY(leaf->at(leaf->fill_ - 1) == i);
            leaf->pop(leaf->fill_ - 1);
            print(leaf);
        }
    }
};

class BranchTest: public TestCase
{
    void run() override
    {
        static constexpr unsigned n = Node::Capacity;
        Local<Head> head[n];
        Local<Node> node[n];
        for (unsigned i = 0; i < n; ++i) {
            head[i]->weight_ = i;
            head[i]->node_ = &node[i];
        }
        Local<Branch> branch;
        for (unsigned i = 0; i < n; ++i) {
            branch->push(branch->fill_, head[i]);
            CC_VERIFY(branch->fill_ == i + 1);
        }
        for (unsigned i = 0; i < n; ++i) {
            CC_VERIFY(branch->at(i)->weight_ == i);
        }
        for (unsigned i = 0; i < n; ++i) {
            branch->pop(branch->fill_ - 1);
        }
        CC_VERIFY(branch->fill_ == 0);

    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(LeafTest);
    CC_TESTSUITE_ADD(BranchTest);

    return TestSuite::instance()->run(argc, argv);
}
