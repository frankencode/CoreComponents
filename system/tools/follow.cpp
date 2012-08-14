#include <ftl/stdio>
#include <ftl/process>

int main(int argc, char** argv)
{
	using namespace ftl;

	if (argc != 2) return 1;
	Ref<File, Owner> file = File::newInstance(argv[1]);
	file->open(File::Read);
	off_t nw = 0;
	Ref<ByteArray, Owner> buf = ByteArray::newInstance(FTL_DEFAULT_BUF_CAPA);
	while (true) {
		if (file->status()->size() > nw) {
			while (true) {
				int nr = file->readAvail(buf);
				if (nr == 0) break;
				nw += nr;
				rawOutput()->write(buf->data(), nr);
			}
		}
		Process::sleep(1);
		file->status()->update();
	}

	return 0;
}
