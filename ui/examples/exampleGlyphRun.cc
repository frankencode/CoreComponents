#include <cc/GlyphRun>
#include <cc/View>

int main()
{
    using namespace cc;

    GlyphRun run{"Hello, world!"};

    View view;

    return
        View{640, 480, &view}
        .paper(Color::White)
        .paint([=]{
            Point pos = 0.5 * (view.size() - Point{run.advance()[0], -run.maxAscender()});
            Painter{view}.showGlyphRun(pos.round(), run);
        })
        .run();
}
