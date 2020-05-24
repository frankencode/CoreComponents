#include <cc/stdio>
#include <cc/debug>

using namespace cc;

class A {
public:
    int x() const { return x_; }
    void setX(int x) { x_ = x; }
    A *operator->() { fout("rw\n"); return this; }
    const A *operator->() const { fout("r\n"); return this; }
    int x_;
};

int main() {
    A a;
    inspect(a->x());
    a->setX(1);
    const A &b = a;
    inspect(b->x());
    return 0;
}
