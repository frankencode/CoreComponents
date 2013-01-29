#include <ftl/stdio>
#include <ftl/process>

int main(int argc, char **argv)
{
	using namespace ftl;

	if (argc != 2) return 1;
	hook<File> file = File::open(argv[1]);
	off_t nw = 0;
	hook<ByteArray> buf = ByteArray::create(FTL_DEFAULT_BUF_CAPA);
	while (true) {
		if (File::status(file->path())->size() > nw) {
			while (true) {
				int nr = file->readAvail(buf);
				if (nr == 0) break;
				nw += nr;
				rawOutput()->write(buf->data(), nr);
			}
		}
		Process::sleep(1);
	}

	return 0;
}
