/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Plot>
#include <cc/math>
#include <cmath>

namespace cc {

Plot::State::State()
{
    pen([=]{
        return
            Pen{theme().primaryTextColor()}
            .lineWidth(sp(1))
            .lineCap(LineCap::Square)
            .lineJoin(LineJoin::Miter);
    });

    paint([this]{
        PointRetrieval retrieval = this->retrieval();
        PointScaling scaling = this->scaling();
        if (!retrieval) return;

        Rect scope = this->scope();
        if (!scope) {
            bool firstTime = true;
            double x0 = 0, y0 = 0, x1 = 0, y1 = 0;
            for (long n = retrieval.pointCount(), i = 0; i < n; ++i) {
                Point a = retrieval.pointAt(i);
                if (scaling) a = scaling.scale(a);
                if (firstTime) {
                    firstTime = false;
                    x0 = x1 = a.x();
                    y0 = y1 = a.y();
                }
                else {
                    if (a.x() < x0) x0 = a.x();
                    else if (x1 < a.x()) x1 = a.x();
                    if (a.y() < y0) y0 = a.y();
                    else if (y1 < a.y()) y1 = a.y();
                }
            }
            scope = Rect{x0, y0, x1, y1};
        }

        double t = pen().lineWidth();
        Point b0 { t / 2, height() - t / 2 };

        Point scale = Point{1, -1} * (size() - Size{t, t}) / scope.size();

        Painter p{this};
        p.setPen(pen());

        bool firstTime = true;
        for (long n = retrieval.pointCount(), i = 0; i < n; ++i) {
            Point a = retrieval.pointAt(i);
            if (scaling) a = scaling.scale(a);
            Point b = b0 + (a - scope.pos()) * scale;
            if (firstTime) {
                firstTime = false;
                p.moveTo(b);
            }
            else {
                p.lineTo(b);
            }
        }
        p.stroke();
    });
}

Plot::Plot():
    View{onDemand<State>}
{}

Plot::Plot(double width, double height):
    View{new State}
{
    size(Size{width, height});
}

Plot &Plot::associate(Out<Plot> self)
{
    return View::associate<Plot>(self);
}

double Plot::symLog(double x)
{
    static constexpr double C = 1 / std::log(10);
    return cc::sgn(x) * std::log10(1 + std::abs(x / C));
}

double Plot::log10(double x)
{
    return x > 0 ? std::log10(x) : 0;
}

} // namespace cc
