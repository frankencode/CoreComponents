#include <ftl/stdio>
#include <ftl/process>

using namespace ftl;

int main(int argc, char** argv)
{
	if (argc != 2) return 1;
	Ref<File, Owner> file = new File(argv[1], File::Read);
	off_t nw = 0;
	Ref<ByteArray, Owner> buf = new ByteArray(FTL_DEFAULT_BUF_CAPA);
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
