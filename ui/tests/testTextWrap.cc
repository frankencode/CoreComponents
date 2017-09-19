/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/ui/Application>
#include <cc/ui/View>
#include <cc/ui/Painter>

using namespace cc;
using namespace cc::ui;

class WrapTest: public Object, public Recipient
{
public:
    static Ref<WrapTest> create(View *view) { return new WrapTest(view); }

private:
    WrapTest(View *view):
        text_(
            "Es war einmal eine Königstochter, die ging hinaus in den Wald und setzte "
            "sich an einen kühlen Brunnen. Sie hatte eine goldene Kugel, die war ihr "
            "liebstes Spielwerk, die warf sie in die Höhe und fing sie wieder in der "
            "Luft und hatte ihre Lust daran. Einmal war die Kugel gar hoch geflogen, "
            "sie hatte die Hand schon ausgestreckt und die Finger gekrümmt, um sie "
            "wieder zufangen, da schlug sie neben vorbei auf die Erde, rollte und rollte "
            "und geradezu in das Wasser hinein."
        ),
        style_(TextStyle::create())
    {
        // style_->setBounding(TextStyle::Wrap);
        style_->setBounding(TextStyle::Justify);
        view->viewResized()->connect(this, &WrapTest::paint);
        paint(view);
    }

    void paint(View *view)
    {
        Ref<Painter> p = view->createPainter();
        Ref<Rect> rect = Rect::copy(view);
        rect->inset(50);
        if (!rect->isValid()) return;
        style_->setBoundary(rect->w());
        p->setTextStyle(style_);
        p->drawText(text_, PlaceTopLeft, rect);
    }

    String text_;
    Ref<TextStyle> style_;
};

int main(int argc, char **argv)
{
    try {
        Application::create(argc, argv);
        Ref<Window> window = Window::open("Test 1, 2, 3...", 640, 480, "#FF8040");
        Ref<WrapTest> wrapTest = WrapTest::create(window->rootView());
        Application::instance()->run();
    }
    catch (Exception &ex) {
        ferr() << ex << nl;
    }
    return 0;
}
