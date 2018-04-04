/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/ui/FontManager>

using namespace cc;
using namespace cc::ui;
using namespace cc::testing;

class TestFontFace: public FontFace
{
public:
    static Ref<TestFontFace> create(
        String family,
        Weight weight = Weight::Normal,
        Slant slant = Slant::Normal,
        Stretch stretch = Stretch::Normal,
        Pitch pitch = Pitch::Variable
    ) {
        return new TestFontFace(
            family, weight, slant, stretch, pitch
        );
    }

    String family() const override { return family_; }
    Weight weight() const override { return weight_; }
    Slant slant() const override { return slant_; }
    Stretch stretch() const override { return stretch_; }
    Pitch pitch() const override { return pitch_; }

private:
    TestFontFace(
        String family,
        Weight weight,
        Slant slant,
        Stretch stretch,
        Pitch pitch
    ):
        family_(family),
        weight_(weight),
        slant_(slant),
        stretch_(stretch),
        pitch_(pitch)
    {}

    String family_;
    Weight weight_;
    Slant slant_;
    Stretch stretch_;
    Pitch pitch_;
};

class TestFontManager: public FontManager
{
public:
    static Ref<TestFontManager> create()
    {
        return new TestFontManager;
    }

    Ref<ScaledFont> selectFont(
        String family,
        double size = 10,
        Weight weight = Weight::Normal,
        Slant slant = Slant::Normal,
        Stretch stretch = Stretch::Normal
    ) const override { return Ref<ScaledFont>{}; }

private:
    Ref<FontFace> openFontFace(String) override { return nullptr; }

    TestFontManager()
    {
        addFontFace( TestFontFace::create("DejaVu Math TeX Gyre") );

        addFontFace( TestFontFace::create("DejaVu Sans") );
        addFontFace( TestFontFace::create("DejaVu Sans", Weight::Normal, Slant::Italic) );
        addFontFace( TestFontFace::create("DejaVu Sans", Weight::Bold) );
        addFontFace( TestFontFace::create("DejaVu Sans", Weight::Bold,   Slant::Italic) );
        addFontFace( TestFontFace::create("DejaVu Sans", Weight::Normal, Slant::Normal, Stretch::SemiCondensed) );
        addFontFace( TestFontFace::create("DejaVu Sans", Weight::Normal, Slant::Italic, Stretch::SemiCondensed) );
        addFontFace( TestFontFace::create("DejaVu Sans", Weight::Bold,   Slant::Normal, Stretch::SemiCondensed) );
        addFontFace( TestFontFace::create("DejaVu Sans", Weight::Bold,   Slant::Italic, Stretch::SemiCondensed) );
        addFontFace( TestFontFace::create("DejaVu Sans", Weight::ExtraLight) );

        addFontFace( TestFontFace::create("DejaVu Sans Mono", Weight::Normal, Slant::Normal, Stretch::Normal, Pitch::Fixed) );
        addFontFace( TestFontFace::create("DejaVu Sans Mono", Weight::Bold,   Slant::Normal, Stretch::Normal, Pitch::Fixed) );
        addFontFace( TestFontFace::create("DejaVu Sans Mono", Weight::Bold,   Slant::Italic, Stretch::Normal, Pitch::Fixed) );
        addFontFace( TestFontFace::create("DejaVu Sans Mono", Weight::Normal, Slant::Italic, Stretch::Normal, Pitch::Fixed) );

        addFontFace( TestFontFace::create("DejaVu Serif") );
        addFontFace( TestFontFace::create("DejaVu Serif", Weight::Bold ) );
        addFontFace( TestFontFace::create("DejaVu Serif", Weight::Normal, Slant::Italic) );
        addFontFace( TestFontFace::create("DejaVu Serif", Weight::Bold,   Slant::Italic) );
        addFontFace( TestFontFace::create("DejaVu Serif", Weight::Normal, Slant::Normal, Stretch::SemiCondensed) );
        addFontFace( TestFontFace::create("DejaVu Serif", Weight::Bold,   Slant::Normal, Stretch::SemiCondensed) );
        addFontFace( TestFontFace::create("DejaVu Serif", Weight::Normal, Slant::Italic, Stretch::SemiCondensed) );
        addFontFace( TestFontFace::create("DejaVu Serif", Weight::Bold,   Slant::Italic, Stretch::SemiCondensed) );
    }
};

class TestFaceLookup: public TestCase
{
    static const FontFace *selectFontFace(
        FontManager *manager,
        String searchPattern,
        Weight searchWeight = Weight::Normal,
        Slant searchSlant = Slant::Normal,
        Stretch searchStretch = Stretch::Normal
    )
    {
        const FontFace *face = manager->selectFontFamily(searchPattern)->selectFontFace(searchWeight, searchSlant, searchStretch);
        fout()
            << searchPattern << " (" << searchWeight << ", " << searchSlant << ", " << searchStretch << ")"
            << " => "
            << face->family() << " (" << face->weight() << ", " << face->slant() << ", " << face->stretch() << ")"
            << nl;
        CC_VERIFY(face->weight() == searchWeight);
        CC_VERIFY(face->slant() == searchSlant);
        CC_VERIFY(face->stretch() == searchStretch);
        return face;
    }

    void run() override
    {
        Ref<FontManager> manager = TestFontManager::create();
        {
            const FontFace *face = selectFontFace(manager, "DejaVu Sans Mono");
            CC_VERIFY(face->family() == "DejaVu Sans Mono");
        }
        {
            const FontFace *face = selectFontFace(manager, "sans");
            CC_VERIFY(face->family() == "DejaVu Sans");
        }
        {
            const FontFace *face = selectFontFace(manager, "DEJA");
            CC_VERIFY(face->family() == "DejaVu Sans");
        }
        {
            const FontFace *face = selectFontFace(manager, "mono");
            CC_VERIFY(face->family() == "DejaVu Sans Mono");
        }
        {
            const FontFace *face = selectFontFace(manager, "tex");
            CC_VERIFY(face->family() == "DejaVu Math TeX Gyre");
        }
        {
            const FontFace *face = selectFontFace(manager, "deja serif");
            CC_VERIFY(face->family() == "DejaVu Serif");
        }
        {
            const FontFace *face = selectFontFace(manager, "serif", Weight::Normal, Slant::Italic);
            CC_VERIFY(face->family() == "DejaVu Serif");
        }
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(TestFaceLookup);

    return TestSuite::instance()->run(argc, argv);
}
