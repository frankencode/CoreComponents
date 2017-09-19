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
#include <cc/ui/FontFace>

using namespace cc;
using namespace cc::ui;

void listFonts()
{
    FontProvider *provider = Application::instance()->fontProvider();

    for (int i = 0; i < provider->fontCount(); ++i)
    {
        FontInfo *info = provider->fontInfo(i);
        fout() << info->family() << (info->fixedWidth() ? " (fixed width)" : "") << nl;

        for (int j = 0; j < info->faceCount(); ++j) {
            FontFace *face = info->faceAt(j);
            fout() << "\t" << Font::stretchName(face->stretch()) << " " << Font::weightName(face->weight()) << (face->italic() ? " Italic" : "") << nl;
        }
    }
}

class TestItem: public Object
{
public:
    static Ref<TestItem> create(Font *font) { return new TestItem(font); }

    Font *font() const { return font_; }
    String text() const { return text_; }

private:
    TestItem(Font *font):
        font_(font),
        text_(
            Format("%% %% %% %%")
            << font->family()
            << Font::weightName(font->weight())
            << Font::stretchName(font->stretch())
            << (font->italic() ? "Italic" : "Regular")
        )
    {
        font_->setSize(20);
    }

    Ref<Font> font_;
    String text_;
};

class RenderTest: public Object, public Recipient
{
public:
    static Ref<RenderTest> create(View *view) { return new RenderTest(view); }

private:
    typedef Map<String, Ref<TestItem> > TestItems;

    RenderTest(View *view):
        testItems_(TestItems::create())
    {
        generateTestItems(false);
        generateTestItems(true);
        paint(view);
        view->viewResized()->connect(this, &RenderTest::paint);
    }

    void generateTestItems(bool italic)
    {
        FontProvider *provider = Application::instance()->fontProvider();
        for (int i = 0; i < provider->fontCount(); ++i) {
            FontInfo *info = provider->fontInfo(i);
            for (int j = 0; j < info->faceCount(); ++j) {
                FontFace *face = info->faceAt(j);
                if (face->italic() != italic) continue;
                Ref<Font> font = provider->selectFont(i);
                font->setItalic(face->italic());
                font->setStretch(face->stretch());
                font->setWeight(face->weight());
                Ref<TestItem> item = TestItem::create(font);
                testItems_->insert(item->text(), item);
            }
        }
    }

    void paint(View *view)
    {
        Ref<Painter> p = view->createPainter();
        Ref<Rect> rect = Rect::create(0, 0, view->w(), view->h());

        for (int i = 0; i < testItems_->count(); ++i) {
            TestItem *item = testItems_->valueAt(i);
            p->setFont(item->font());
            rect->resize(rect->w(), FontMetrics::get(item->font())->lineHeight());
            p->drawText(item->text(), PlaceBottomLeft, rect);
            rect->moveTo(0, rect->y() + rect->h());
        }
    }

    Ref<TestItems> testItems_;
};

int main(int argc, char **argv)
{
    try {
        Application::create(argc, argv);
        listFonts();
        Ref<Window> window = Window::open("Test 1, 2, 3...", 640, 480, Color("#FF8040"));
        Ref<RenderTest> renderTest = RenderTest::create(window->rootView());
        Application::instance()->run();
    }
    catch (Exception &ex) {
        ferr() << ex << nl;
    }
    return 0;
}
