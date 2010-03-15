#include <pona/stdio>
#include <pona/strings>

namespace pona
{

int main(int argc, char** argv)
{
	if (argc != 2) {
		print("Usage: %% [FILE]\n", File(argv[0]).name());
		return 0;
	}
	
	File file(argv[1], File::Read);
	String text(file.size());
	file.read(text->data(), text->size());
	if (!text.valid()) {
		print("Input data is not conforming to UTF8 encoding.\n");
		return 1;
	}
	if (text == "") {
		print("Input file empty.\n");
		return 2;
	}
	
	print("text = \"%%\"\n", text->data());
	
	int nf = 0, nb = 0; // forward, backward character count
	for (String::Index i = text.first(); i.valid(); ++i) ++nf;
	for (String::Index i = text.last(); i.valid(); --i) ++nb;
	
	/*print("======================\n");
	
	for (String::Index i = text.last(); i.valid(); --i) {
		uchar_t ch = text.get(i);
		print("text[0x%%] = %% / '%%'\n", i.pos() - i.data(), ch, Char(ch));
	}
	
	print("----------------------\n");
	
	for (String::Index i = text.first(); i.valid(); ++i) {
		uchar_t ch = text.get(i);
		print("text[0x%%] = %% / '%%'\n", i.pos() - i.data(), ch, Char(ch));
	}
	
	print("nf, nb = %%, %%\n", nf, nb);
	if (nf != nb) {
		print("Test failed: nf != nb\n");
		return 3;
	}
	
	print("----------------------\n");*/
	
	{
		String s("123x");
		const char* pattern = "x";
		String::Index i = s.find(pattern);
		print("Find \"%%\" in \"%%\": %% (valid = %%)\n", pattern, s->data(), i.pos() - i.data(), i.valid());
		/*print("s == String(s.split(pattern), pattern): %%\n", s == String(s.split(pattern), pattern));
		print("s.split(pattern)->length() = %%\n", s.split(pattern)->length());
		print("String(s.split(pattern), pattern) = '%%'\n", String(s.split(pattern), pattern).data());*/
	}
	
	Ref<StringList, Owner> lines = text.split("\n");
	print("Number of lines: %%\n", lines->length());
	int j = 0;
	for (StringList::Index i = lines->first(); lines->def(i); ++i, ++j)
		print("%%: '%%' (%%)\n", j, lines->at(i)->data(), lines->at(i)->size());
	
	return 0;
}

} // namespace pona

int main(int argc, char** argv)
{
	return pona::main(argc, argv);
}
