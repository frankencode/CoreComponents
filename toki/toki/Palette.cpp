#include "Palette.h"

namespace fluxtoki
{

Palette::Palette()
{}

void Palette::define()
{
	className("Palette");
	protocol()->minCount(1);
	protocol()->define<Style>();
}

/*
toki::Default,
toki::CurrentLine,
toki::Cursor,
toki::Selection,
toki::Match,
toki::LineNumber,
toki::CurrentLineNumber
*/

} // namespace fluxtoki
