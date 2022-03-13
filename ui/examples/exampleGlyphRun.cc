#include <cc/ui/Application>
#include <cc/ui/GlyphRun>
#include <cc/ui/Painter>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    GlyphRun run{"Hello, world!"};

    View view{640, 480};
    view.paper(Color::White);
    view.paint([=]{
        Painter p{view};
        Point pos = 0.5 * (view.size() - Point{run.advance()[0], -run.maxAscender()});
        p.showGlyphRun(pos.round(), run);
    });

    Window{view}.show();

    return Application{}.run();
}
