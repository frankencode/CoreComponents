/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <math.h>
#include <cc/stdio>
#include <cc/str>
#include <cc/ui/Application>
#include <cc/ui/View>
#include <cc/ui/Painter>
#include <cc/ui/TypeSetter>

using namespace cc;
using namespace cc::ui;

class RenderTest: public Object, public Recipient
{
public:
    static Ref<RenderTest> create(View *view) { return new RenderTest(view); }

private:
    RenderTest(View *view):
        typeSetter_(TypeSetter::create()),
        font_(Font::select("DejaVu Sans Mono")),
        text_("Ref<TypeSetter>, The quick brown fox jumps over the lazy dog")
    {
        // font_->setItalic(true);
        font_->setHinting(true);
        // font_->setWeight(Font::SemiBold);
        view->viewResized()->connect(this, &RenderTest::paint);
        paint(view);
    }

    void paint(View *view)
    {
        Ref<Painter> p = view->createPainter();
        Ref<Rect> rect = Rect::create();
        double y = 0;
        for (double size = 9; size >= 8; size -= 0.1) {
            font_->setSize(size);
            Ref<FontMetrics> metrics = FontMetrics::get(font_);
            rect->resize(rect->w(), metrics->lineHeight());
            p->setFont(font_);
            for (int i = 0; i < 2; ++i) {
                p->drawText(fixed(size, 1) + ": " + text_, PlaceAuto, rect);
                y += ::round(metrics->lineHeight()) + 2;
                rect->moveTo(0, y);
            }
            y += 10;
            rect->moveTo(0, y);
        }
    }

    Ref<TypeSetter> typeSetter_;
    Ref<Font> font_;
    String text_;
};

int main(int argc, char **argv)
{
    try {
        Application::create(argc, argv);
        Ref<Window> window = Window::open("Test 1, 2, 3...", 640, 480, "#FFFFFF");
        Ref<RenderTest> renderTest = RenderTest::create(window->rootView());
        Application::instance()->run();
    }
    catch (Exception &ex) {
        ferr() << ex << nl;
    }
    return 0;
}
