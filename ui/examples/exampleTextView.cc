#include <cc/Application>
#include <cc/TextView>
#include <cc/File>
#include <cc/DEBUG>

int main(int argc, char *argv[])
{
    using namespace cc;

    String text;
    if (argc > 1) {
        text = File{argv[1]}.map();
        // CC_INSPECT(text);
    }
    else {
        text =
            "(1) Es war einmal eine Königstochter, die ging hinaus in den Wald und setzte sich an einen kühlen Brunnen.\n"
            "(2) Sie hatte eine goldene Kugel, die war ihr liebstes Spielwerk, die warf sie in die Höhe und fing sie wieder in der Luft und hatte ihre Lust daran.\n"
            "(3) Einmal war die Kugel gar hoch geflogen, sie hatte die Hand schon ausgestreckt und die Finger gekrümmt, um sie wieder zufangen, da schlug sie neben vorbei auf die Erde, rollte und rollte und geradezu in das Wasser hinein.\n";
    }

    Window{
        TextView{648, 480}
        .text(text)
    }.show();

    return Application{}.run();
}
