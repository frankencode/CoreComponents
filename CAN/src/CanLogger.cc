/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/CanLogger>
#include <cc/IoStream>
#include <cc/System>
#include <cc/Thread>
#include <cc/Format>
#include <cc/Map>
#include <cc/debugging>
#include <cc/str>

namespace cc {

struct CanLogger::State final: public Object::State
{
    State(const CanMedia &media):
        media_{media}
    {}

    ~State()
    {
        if (thread_) {
            thread_.wait();
            thread_ = Thread{};
        }
    }

    void addAnnotator(CanAnnotator &&annotator)
    {
        assert(!thread_);
        annotators_.append(annotator);
    }

    void setOutput(const Stream &output)
    {
        assert(!thread_);
        output_ = output;
    }

    void start()
    {
        if (!output_) output_ = IoStream::output();

        thread_ = Thread{[this]{ run(); }};
        thread_.start();
    }

    void run()
    {
        Map<uint32_t, double> lastTimes;

        double t0 = -1;

        for (CanFrame frame; media_.read(&frame);)
        {
            double t = System::now();
            if (t0 < 0) t0 = t;
            t -= t0;

            double dt = 0, tl = 0;
            if (lastTimes.lookup(frame.canId(), &tl)) {
                dt = t - tl;
            }
            lastTimes.establish(frame.canId(), t);

            Format f;
            f << fixed(t, 3, 3) << " " << fixed(dt, 3, 3) << " -- " << frame << nl;

            String info;

            for (CanAnnotator &annotator: annotators_) {
                info = annotator.annotate(frame);
                if (info != "") {
                    f << nl << info.indented("  ") << nl << nl;
                    break;
                }
            }

            output_.write(f);
        }
    }

    Thread thread_;
    CanMedia media_;
    List<CanAnnotator> annotators_;
    Stream output_;
};

CanLogger::CanLogger(const CanMedia &media):
    Object{new State{media}}
{}

CanLogger &CanLogger::addAnnotator(CanAnnotator &&annotator)
{
    me().addAnnotator(std::move(annotator));
    return *this;
}

CanLogger &CanLogger::setOutput(const Stream &output)
{
    me().setOutput(output);
    return *this;
}

void CanLogger::start()
{
    me().start();
}

CanLogger::State &CanLogger::me()
{
    return Object::me.as<State>();
}

} // namespace cc
