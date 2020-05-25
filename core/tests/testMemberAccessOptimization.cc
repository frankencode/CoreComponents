#include <cc/stdio>
#include <cc/debug>

using namespace cc;

template<class T>
class access
{
public:
    access(T &x): x_(x) {}

    operator T &() { fout("access RW\n"); return x_; }
    operator const T &() const { fout("access R\n"); return x_; }

    access &operator=(const T &x) { fout("access RW=\n"); x_ = x; return *this; }

private:
    T &x_;
};

class A {
public:
    int x() const { fout("R\n"); return x_; }
    void setX(int x) { access<int>{x_} = x; }

    A *operator->() { fout("->RW\n"); return this; }
    const A *operator->() const { fout("->R\n"); return this; }

    access<int> operator[](size_t) { fout("RW[]\n"); return x_; }
    const int &operator[](size_t) const { fout("R[]\n"); return x_; }

private:
    int x_ { 0 };
};

int main() {
    A a;
    a[0] = 1;
    fout("\n");
    inspect(a->x());
    fout("\n");
    inspect(a[0]);
    return 0;
}
