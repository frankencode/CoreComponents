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

class WindowLog: public Object, public Recipient
{
public:
    static Ref<WindowLog> create(Window *window) { return new WindowLog(window); }

private:
    WindowLog(Window *window) {
        window->windowMoved()->connect(this, &WindowLog::windowMoved);
        window->windowResized()->connect(this, &WindowLog::windowResized);
    }
    void windowMoved(Window *window) {
        fout() << "moved to " << window->x() << ", " << window->y() << nl;
    }
    void windowResized(Window *window) {
        fout() << "resized to " << window->w() << ", " << window->h() << nl;
    }
};

class PlacementTest: public Object, public Recipient
{
public:
    static Ref<PlacementTest> create(View *view) { return new PlacementTest(view); }

private:
    PlacementTest(View *view)
    {
        view->viewResized()->connect(this, &PlacementTest::paint);
        paint(view);
    }

    void paint(View *view)
    {
        Ref<Painter> p = view->createPainter();
        p->fillRect(0, view->h() / 2, view->w(), 1, "#0080FF");
        p->drawText("Top Left", PlaceTopLeft);
        p->drawText("Top Center", PlaceTopCenter);
        p->drawText("Top Right", PlaceTopRight);
        p->drawText("Center Left", PlaceCenterLeft);
        p->drawText("Center", PlaceCenter);
        p->drawText("Center Right", PlaceCenterRight);
        p->drawText("Bottom Left", PlaceBottomLeft);
        p->drawText("Bottom Center", PlaceBottomCenter);
        p->drawText("Bottom Right", PlaceBottomRight);
    }
};

int main(int argc, char **argv)
{
    try {
        Application::create(argc, argv);
        Ref<Window> window = Window::open("Test 1, 2, 3...", 640, 480, "#FF8040");
        Ref<PlacementTest> placementTest = PlacementTest::create(window->rootView());
        // Ref<WindowLog> log = WindowLog::create(window);
        Application::instance()->run();
    }
    catch (Exception &ex) {
        ferr() << ex << nl;
    }
    return 0;
}
