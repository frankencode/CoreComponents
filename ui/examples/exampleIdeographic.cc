#include <cc/Picture>

int main()
{
    using namespace cc;

    return
          // Picture{Ideographic::AccountPlus, sp(24)}
          // Picture{Ideographic::Check, sp(24)}
          // Picture{Ideographic::CheckAll, sp(24)}
          // Picture{Ideographic::CheckCircle, sp(24)}
          // Picture{Ideographic::CheckCircleOutline, sp(24)}
        // Picture{Ideographic::CheckOutline, sp(24)}
        // Picture{Ideographic::CheckboxBlankOutline, sp(24)}
          Picture{Ideographic::CheckboxIntermediate, sp(24)}
        // Picture{Ideographic::CheckboxMarked, sp(24)}
        // Picture{Ideographic::CheckboxMultipleMarked, sp(24)}

        .paper("white")
        .run();
}
