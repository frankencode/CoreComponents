#include <ftl/stdio>
#include <ftl/binary>

int main(int argc, char** argv)
{
	using namespace ftl;
	
	if (argc != 3) {
		print("Usage: cat in_file | replace [PATTERN] [REPLACEMENT] > out_file\n\n");
		return 1;
	}
	String pattern = argv[1];
	String replacement = argv[2];
	pattern->expandInsitu();
	replacement->expandInsitu();
	
	Ref<ByteDecoder, Owner> source = new ByteDecoder(rawInput());
	Ref<ByteEncoder, Owner> sink = new ByteEncoder(rawOutput());
	int k = 0;
	char ch = 0;
	while (source->read(&ch)) {
		if (pattern->at(k) == ch) {
			++k;
			if (k == pattern->length()) {
				for (int j = 0, n = replacement->length(); j < n; ++j)
					sink->write(replacement->at(j));
				k = 0;
			}
		}
		else if (k > 0) {
			for (int j = 0; j < k; ++j)
				sink->write(pattern->at(j));
			k = 0;
		}
		else {
			sink->write(ch);
		}
	}
	
	return 0;
}
