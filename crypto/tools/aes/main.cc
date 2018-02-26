/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/exceptions>
#include <cc/Arguments>
#include <cc/File>
#include <cc/ByteSink>
#include <cc/ByteSource>
#include <cc/Crc32Sink>
#include <cc/crypto/AesCipher>
#include <cc/crypto/BlockCascade>
#include <cc/crypto/CipherSink>
#include <cc/crypto/CipherSource>
#include "Readline.h"

using namespace cc;
using namespace cc::crypto;
using namespace ccaes;

String normalizePassword(String password)
{
    String s = String::create(16);
    mutate(s)->fill(' ');
    mutate(s)->write(password);
    return s;
}

int main(int argc, char **argv)
{
    String toolName = String(argv[0])->fileName();
    bool encipher = !toolName->contains("un");

    try {
        Ref<Arguments> arguments = Arguments::parse(argc, argv);
        arguments->validate(VariantMap::create());

        Ref<const StringList> items = arguments->items();

        bool viewMode = stdErr()->isatty() && !stdOut()->isatty();
        bool filterMode = items->count() == 0 && !stdIn()->isatty();
        if (items->count() == 0 && stdIn()->isatty()) throw UsageError("No input data");

        if (encipher) {
            String password, retype;
            while (true) {
                password = Readline::getPassword("Enter password: ");
                String retype = Readline::getPassword("Enter again: ");
                if (password == retype) break;
                fout() << "Passwords differ, try again" << nl;
            }

            password = normalizePassword(password);

            Ref<Stream> random = File::open("/dev/random");

            for (String path: items)
            {
                String contentKey = random->readSpan(16);
                String encipheredContentKey = String::allocate(contentKey->count());
                AesCipher::create(password)->encode(contentKey, mutate(encipheredContentKey));

                Ref<BlockCipher> cipher = BlockCascade::create(AesCipher::create(contentKey));

                String fileName = path->fileName();
                String outPath = fileName + ".aes";
                if (File::exists(outPath)) {
                    ferr() << outPath << ": file exists, skipping" << nl;
                    continue;
                }
                Ref<Stream> source = File::open(path);
                Ref<Stream> sink;
                if (viewMode)
                    sink = stdOut();
                else
                    sink = File::open(outPath, OpenMode::Create|OpenMode::Truncate|OpenMode::WriteOnly);
                sink->write(encipheredContentKey);

                Ref<CipherSink> cipherSink = CipherSink::open(cipher, sink, random);
                {
                    Ref<ByteSink> header = ByteSink::open(cipherSink, mutate(String::allocate(0x100)));
                    String challenge = random->readSpan(16);
                    header->write(challenge);
                    header->writeUInt32(crc32(challenge));
                    header->writeInt32(fileName->count());
                    header->write(fileName);
                    header->writeUInt64(FileStatus::read(path)->size());
                }
                source->transferTo(cipherSink);

                File::unlink(path);
            }
        }
        else {
            Ref<SystemStream> stdInSaved;
            Ref<SystemStream> stdOutSaved;
            if (items->count() == 0) {
                items = StringList::create() << "";
                stdInSaved = SystemStream::duplicate(stdIn());
                stdOutSaved = SystemStream::duplicate(stdOut());
                stdErr()->duplicateTo(stdIn());
                stdErr()->duplicateTo(stdOut());
            }

            String password = Readline::getPassword("Enter password: ");
            password = normalizePassword(password);

            if (viewMode) fout() << nl;

            for (String path: items)
            {
                Ref<Stream> source;
                if (path == "")
                    source = stdInSaved;
                else
                    source = File::open(path);

                String contentKey = String::allocate(16);
                String encipheredContentKey = source->readSpan(16);
                AesCipher::create(password)->decode(encipheredContentKey, mutate(contentKey));

                Ref<BlockCipher> cipher = BlockCascade::create(AesCipher::create(contentKey));
                Ref<CipherSource> cipherSource = CipherSource::open(cipher, source);
                String origName;
                uint64_t origSize = 0;
                {
                    Ref<ByteSource> header = ByteSource::open(cipherSource, mutate(String::allocate(1)));
                    String challenge = header->readSpan(16);
                    if (crc32(challenge) != header->readUInt32()) throw UsageError("Invalid password or invalid file");
                    int nameLength = header->readInt32();
                    origName = header->readSpan(nameLength);
                    origSize = header->readUInt64();

                    if (!viewMode && !filterMode) fout() << origName << nl;
                }
                Ref<Stream> sink;
                if (viewMode || filterMode) {
                    if (stdOutSaved)
                        sink = stdOutSaved;
                    else
                        sink = stdOut();
                }
                else {
                    if (File::exists(origName)) {
                        ferr() << origName << ": file exists, skipping" << nl;
                        continue;
                    }
                    sink = File::open(origName, OpenMode::Create|OpenMode::Truncate|OpenMode::WriteOnly);
                }
                cipherSource->transferSpanTo(origSize, sink);
            }
        }
    }
    catch (UsageError &ex) {
        ferr() << ex.message() << nl;
        return 1;
    }
    catch (HelpError &) {
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
