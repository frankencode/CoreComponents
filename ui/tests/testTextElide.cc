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

class ElideTest: public Object, public Recipient
{
public:
    static Ref<ElideTest> create(View *view) { return new ElideTest(view); }

private:
    ElideTest(View *view):
        text_("7bf91a7cbfed7304f0d4c967756bb466"),
        style_(TextStyle::create())
    {
        // style_->setBounding(TextStyle::ElideBack);
        style_->setBounding(TextStyle::ElideFront);
        view->viewResized()->connect(this, &ElideTest::paint);
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
        p->drawText(text_, PlaceCenterLeft, rect);
    }

    String text_;
    Ref<TextStyle> style_;
};

int main(int argc, char **argv)
{
    try {
        Application::create(argc, argv);
        Ref<Window> window = Window::open("Test 1, 2, 3...", 640, 480, "#FF8040");
        Ref<ElideTest> elideTest = ElideTest::create(window->rootView());
        Application::instance()->run();
    }
    catch (Exception &ex) {
        ferr() << ex << nl;
    }
    return 0;
}
