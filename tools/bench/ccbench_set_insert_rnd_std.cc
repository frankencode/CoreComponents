#include <cc/System>
#include <cc/Random>
#include <cc/stdio>
#include <array>
#include <set>

using namespace cc;

int64_t benchmark(std::function<void()> &&run, std::function<void()> &&init = std::function<void()>{}, int repetition = 1)
{
    double dt_min = 0;
    for (int i = 0; i < repetition; ++i) {
        if (init) init();
        double dt = System::now();
        run();
        dt = System::now() - dt;
        if (dt < dt_min || dt_min <= 0) dt_min = dt;
    }
    return static_cast<int64_t>(std::round(dt_min * 1e6));
}

/** Print \a data as Python list
  */
void printArray(const char *label, const auto &data)
{
    Format f;
    for (auto x: data) {
        f << x << ", ";
    }
    if (f.count() > 0) f.popBack();
    print("%% = [ %% ]\n", label, f.join());
}

int main(int argc, char *argv[])
{
    std::array<int, 12> counts { 1000, 5000, 10000, 50000, 100000, 500000, 1000000, 2000000, 3000000, 4000000, 5000000, 6000000 };
    std::array<int64_t, 12> durations;

    const int n = counts[counts.size() - 1];

    auto items = Array<long>::allocate(n);

    fout() << "Test preparation: generate shuffled random numbers... ";
    {
        double t = System::now();

        Random random { 0 };

        for (int i = 0; i < n; ++i) {
            items[i] = random();
        }

        for (int i = 0; i < 3; ++i) {
            random.shuffle(items);
        }

        t = System::now() - t;
        fout() << std::round(t * 1000) << " ms\n";
    }

    std::set<long> numbers;

    for (unsigned k = 0; k < counts.size(); ++k) {
        const int n = counts[k];
        int64_t dt = benchmark(
            [&]{
                for (int i = 0; i < n; ++i) {
                    numbers.insert(items[i]);
                }
            },
            [&]{
                numbers.clear();
            }
        );
        durations[k] = dt;
        fout() << n << "\trandom lookups to std::set<int> cost\t" << dt << " us\n";
    }

    printArray("x", counts);
    printArray("y", durations);

    return 0;
}
