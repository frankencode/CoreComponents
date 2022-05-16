#include <cc/Array>
#include <cc/Complex>
#include <cc/System>
#include <cc/stdio>
#include <cc/fft>

int main(int argc, char *argv[])
{
    using namespace cc;

    Array<Complex> v = Array<Complex>::allocate(64);
    Array<Complex> w = Array<Complex>::allocate(64);
    for (int i = 0; i < v.size(); ++i) v[i] = i;
    for (int i = 0; i < v.size(); ++i) fout() << i << ": " << fixed(v[i], 6) << nl;
    fout() << nl;
    fft(&w, v);
    auto t = System::now();
    fft(&v);
    t = System::now() - t;
    for (int i = 0; i < v.size(); ++i) fout() << i << ": " << fixed(v[i], 6) << nl;
    fout() << nl;
    ift(&v);
    for (int i = 0; i < v.size(); ++i) fout() << i << ": " << fixed(v[i], 6) << nl;
    fout() << nl;
    fout() << fixed(t*1e3, 3) << "ms" << nl;
    return 0;
}
