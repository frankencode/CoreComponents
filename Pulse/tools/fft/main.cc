#include <cc/Application>
#include <cc/Plot>
#include <cc/pulse/Context>
#include <cc/pulse/InputStream>
#include <cc/Thread>
#include <cc/Channel>
#include <cc/Complex>
#include <cc/CircularBuffer>
#include <cc/Color>
#include <cc/HexDump>
#include <cc/fft>
#include <cc/stdio>
#include <cc/DEBUG>

int main(int argc, char *argv[])
{
    using namespace cc;

    String toolName = String{argv[0]}.baseName();

    int exitCode = 0;

    Channel<Bytes> tap;

    Thread pulseWorker {
        [toolName, &tap, &exitCode]{
            using namespace cc::pulse;

            MainLoop mainLoop;
            Context context{mainLoop, toolName};
            InputStream stream{context};

            context.connect([&]{
                context.requestServerInfo([&](const ServerInfo &info)
                {
                    String target = info.defaultSinkName() + ".monitor";
                    CC_INSPECT(target);

                    stream.incoming([&](const Bytes &data){
                        // fout() << hexdump(data);
                        if (!tap.write(data.copy())) {
                            mainLoop.quit();
                        }
                    });

                    stream.connect(target);
                });
            });
            exitCode = mainLoop.run();
            tap.close();
        }
    };

    Plot plot;

    Window{
        Plot{}
        .associate(&plot)
        // .scaling(Plot::log10)
        .points({
            Point{0,   0},
            Point{100, 100}
        })
        .pen(
            Pen{}
            .color({0x00, 0x00, 0x80})
            .lineWidth(sp(1))
            .lineCap(LineCap::Round)
            .lineJoin(LineJoin::Round)
        )
        .size(gu(80), gu(60))
    }.show();

    Thread fftWorker {
        [&tap, &plot]{
            const long n = 2048; // buffer size
            const long l = n / 2; // buffer overlap
            auto buffer = CircularBuffer<int16_t>::allocate(n);
            for (Bytes data; tap.read(&data);) {
                for (long j = 0, m = data.itemCount<int16_t>(); j < m; ++j) {
                    buffer.pushBack(data.item<int16_t>(j));
                    if (buffer.fill() == n) {
                        auto v = Array<Complex>::allocate(n);
                        for (long k = 0; k < n; ++k) {
                            v[k] = buffer[k];
                        }
                        for (long k = 0; k < l; ++k) { // FIXME: need a "dropFront(amount)" method for CircularBuffer
                            buffer.popFront();
                        }
                        fft(&v);
                        Application{}.postEvent([s=v, &plot]{
                            const long m = n / 2;
                            Array<Point> w = Array<Point>::allocate(m);
                            double b = 0, c = 0;
                            for (long i = 0; i < m; ++i) {
                                double a = std::abs(s[i]);
                                w[i] = Point{double(i), a};
                                if (b < a) b = a;
                                c += a;
                            }
                            plot.points(w);

                            if (b == 0) {
                                // plot.paper(Color::White);
                            }
                            else {
                                double q = 1 - c / (b * m);
                                double h = (q - 0.8) * 5;
                                if (h < 0) h = 0;
                                if (h > 1) h = 1;

                                h *= 270;

                                // CC_INSPECT(m);
                                // CC_INSPECT(q);
                                // CC_INSPECT(h);

                                plot.paper(Color::fromHue(h));
                            }
                        });
                    }
                }
            }
        }
    };

    pulseWorker.start();
    fftWorker.start();

    Application{}.run();

    tap.close();

    pulseWorker.wait();
    fftWorker.wait();

    return exitCode;
}
