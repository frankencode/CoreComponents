#include <cc/AesBlockCipher>
#include <cc/CbcBlockCipher>
#include <cc/BlockCipherSink>
#include <cc/BlockCipherSource>
#include <cc/Crc32Sink>
#include <cc/Arguments>
#include <cc/ByteSink>
#include <cc/ByteSource>
#include <cc/FileInfo>
#include <cc/File>
#include <cc/stdio>
#include <stdio.h>
#include <readline/readline.h>
#include <unistd.h>
#include <termios.h>

namespace cc {

String getPassword(const String &prompt)
{
    IoStream::input().echo(false);

    char *line = readline(prompt);
    String text { line };
    ::free((void *)line);

    IoStream::input().echo(true);

    return text;
}

String normalizePassword(const String &password)
{
    String s = String::allocate(16, ' ');
    password.copyTo(&s);
    return s;
}

} // namespace cc

int main(int argc, char *argv[])
{
    using namespace cc;

    String toolName = String{argv[0]}.baseName();
    bool encipher = !toolName.contains("un");

    try {
        List<String> items = Arguments{argc, argv}.read();

        bool viewMode = IoStream::error().isInteractive() && !IoStream::output().isInteractive();
        bool filterMode = items.count() == 0 && !IoStream::input().isInteractive();
        if (items.count() == 0 && IoStream::input().isInteractive()) throw UsageError{"No input data"};

        if (encipher) {
            String password, retype;
            while (true) {
                password = getPassword("Enter password: ");
                String retype = getPassword("Enter again: ");
                if (password == retype) break;
                fout() << "Passwords differ, try again" << nl;
            }

            password = normalizePassword(password);

            File random{"/dev/random"};

            for (String path: items)
            {
                String contentKey = random.readSpan(16);
                String encipheredContentKey = String::allocate(contentKey.count());
                AesBlockCipher{password}.encode(contentKey, &encipheredContentKey);

                CbcBlockCipher cipher{AesBlockCipher{contentKey}};

                String fileName = path.fileName();
                String outPath = fileName + ".aes";
                if (File::exists(outPath)) {
                    ferr() << outPath << ": file exists, skipping" << nl;
                    continue;
                }
                Stream source = File{path};
                Stream sink;
                if (viewMode) {
                    sink = IoStream::output();
                }
                else {
                    sink = File{outPath, FileOpen::Create|FileOpen::Truncate|FileOpen::WriteOnly};
                }

                sink.write(encipheredContentKey);

                BlockCipherSink cipherSink{sink, cipher, random};
                {
                    ByteSink header{cipherSink, Bytes::allocate(0x100)};
                    String challenge = random.readSpan(16);
                    header.write(challenge);
                    header.writeUInt32(crc32(challenge));
                    header.writeInt32(fileName.count());
                    header.write(fileName);
                    header.writeUInt64(FileInfo{path}.size());
                }
                source.transferTo(cipherSink);

                File::unlink(path);
            }
        }
        else {
            IoStream inputSaved;
            IoStream outputSaved;
            if (items.count() == 0) {
                items.append("");
                inputSaved = IoStream::input().duplicate();
                outputSaved = IoStream::output().duplicate();
                IoStream::error().duplicateTo(IoStream::input());
                IoStream::error().duplicateTo(IoStream::output());
            }

            String password = getPassword("Enter password: ");
            password = normalizePassword(password);

            if (viewMode) fout() << nl;

            for (String path: items)
            {
                Stream source;
                if (path == "")
                    source = inputSaved;
                else
                    source = File{path};

                String contentKey = String::allocate(16);
                String encipheredContentKey = source.readSpan(16);
                AesBlockCipher{password}.decode(encipheredContentKey, &contentKey);

                CbcBlockCipher cipher{AesBlockCipher{contentKey}};
                BlockCipherSource cipherSource{source, cipher};
                String origName;
                uint64_t origSize = 0;
                {
                    ByteSource header{cipherSource, String::allocate(1)};
                    String challenge = String::allocate(16);
                    header.read(&challenge);
                    if (crc32(challenge) != header.readUInt32()) throw UsageError{"Invalid password or invalid file"};
                    int nameLength = header.readInt32();
                    origName = String::allocate(nameLength);
                    header.read(&origName);
                    origSize = header.readUInt64();

                    if (!viewMode && !filterMode) fout() << origName << nl;
                }
                Stream sink;
                if (viewMode || filterMode) {
                    if (outputSaved)
                        sink = outputSaved;
                    else
                        sink = IoStream::output();
                }
                else {
                    if (File::exists(origName)) {
                        ferr() << origName << ": file exists, skipping" << nl;
                        continue;
                    }
                    sink = File{origName, FileOpen::Create|FileOpen::Truncate|FileOpen::WriteOnly};
                }
                cipherSource.transferTo(sink, origSize);
            }
        }
    }
    catch (UsageError &ex) {
        ferr() << ex << nl;
        return 1;
    }
    catch (HelpRequest &) {
        if (encipher) {
            fout(
                "Usage: %% [FILE] ...\n"
                "Encipher each FILE\n"
            ) << toolName;
        }
        else {
            fout(
                "Usage: %% [FILE] ...\n"
                "Decipher each FILE\n"
            ) << toolName;
        }
    }

    return 0;
}
