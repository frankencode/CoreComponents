#include <cc/ui/Application>
#include <cc/ui/View>
#include <cc/ui/FontManager>
#include <cc/DEBUG>

int main(int argc, char **argv)
{
    using namespace cc;
    using namespace cc::ui;

    String fontPath = "<default font>";
    if (argc > 1) {
        fontPath = argv[1];
        FontManager::instance()->addPath(fontPath);
    }

    const double margins = 30;
    const double fontSize = 20;

    Ref<const GlyphRun> glyphRun {
        GlyphRun::typeset(
            #if 0
            "Tοσοῦτον δ' ἀπολέλοιπεν ἡ πόλις ἡμῶν περὶ τὸ φρονεῖν καὶ "
            "λέγειν τοὺς ἄλλους ἀνθρώπους, ὥσθ' οἱ ταύτης μαθηταὶ τῶν "
            "ἄλλων διδάσκαλοι γεγόνασι, καὶ τὸ τῶν Ἑλλήνων ὄνομα πεποίηκε "
            "μηκέτι τοῦ γένους ἀλλὰ τῆς διανοίας δοκεῖν εἶναι, καὶ μᾶλλον "
            "Ἕλληνας καλεῖσθαι τοὺς τῆς παιδεύσεως τῆς ἡμετέρας ἢ τοὺς τῆς "
            "κοινῆς φύσεως μετέχοντας. (Isocrates' speech)"
            #endif
            #if 0
            "Молчи, скрывайся и таи "
            "И чувства, и мечты свои - "
            "Пускай в душевной глубине "
            "Встают и заходят оне "
            "Безмолвно, как звезды в ночи, - "
            "Любуйся ими - и молчи. "
            "Как сердцу высказать себя? "
            "Другому как понять тебя? "
            "Поймет ли он, чем ты живешь? "
            "Мысль изреченная есть ложь. "
            "Взрывая, возмутишь ключи, - "
            "Питайся ими - и молчи. "
            "Лишь жить в себе самом умей - "
            "Есть целый мир в душе твоей "
            "Таинственно-волшебных дум; "
            "Их оглушит наружный шум, "
            "Дневные разгонят лучи, - "
            "Внимай их пенью - и молчи!.. "
            #endif
            #if 0
            "古池や蛙飛び込む水の音ふるいけやかわずとびこむみずのおと (Bashō's old pond)",
            #endif
            #if 1
            "Es war einmal eine Königstochter, die ging hinaus in den Wald und setzte "
            "sich an einen kühlen Brunnen. Sie hatte eine goldene Kugel, die war ihr "
            "liebstes Spielwerk, die warf sie in die Höhe und fing sie wieder in der "
            "Luft und hatte ihre Lust daran. Einmal war die Kugel gar hoch geflogen, "
            "sie hatte die Hand schon ausgestreckt und die Finger gekrümmt, um sie "
            "wieder zufangen, da schlug sie neben vorbei auf die Erde, rollte und rollte "
            "und geradezu in das Wasser hinein."
            #endif
            #if 0
            "人之初，性本善。性相近，习相远。苟不教，性乃迁。教之道，贵以专。昔孟母，择邻处。子不学，断机杼，窦燕山，有义方，教五子，名俱扬。养不教，父之过。教不严，师之惰。 (Confucius)"
            #endif
            ,
            Font{"Noto Sans CJK JP", fontSize}
        )
    };

    View view{640, 480};

    Property< Ref<const GlyphRun> > wrappedRun;
    #if 0 // FIXME
    wrappedRun <<[=]{
        CC_DEBUG;
        return glyphRun->wrap(view->size()[0] - 2 * margins, TextAlign::Justify);
    };
    #endif

    view->paint <<[=]{
        auto wrappedRun = glyphRun->wrap(view->size()[0] - 2 * margins, TextAlign::Justify);
        Painter p{view};
        p->setSource(Color{0xE0F0FF});
        p->rectangle(Point{0, 0}, Size{margins, view->size()[1]});
        p->rectangle(Point{view->size()[0]-margins, 0}, Size{margins, view->size()[1]});
        p->fill();
        p->setSource(Color{0x000000});
        p->showGlyphRun(Point{margins, margins + fontSize}, wrappedRun);
    };

    Window::open(view, fontPath, WindowMode::Accelerated|WindowMode::VSync);

    return Application{}->run();
}
