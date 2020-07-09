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

String normalizePassword(const String &password)
{
    String s = String::create(16);
    mutate(s)->fill(' ');
    mutate(s)->write(password);
    return s;
}

int main(int argc, char **argv)
{
    String toolName = String{argv[0]}->fileName();
    bool encipher = !toolName->contains("un");

    try {
        StringList items = Arguments{argc, argv}->read();

        bool viewMode = stdErr()->isatty() && !stdOut()->isatty();
        bool filterMode = items->count() == 0 && !stdIn()->isatty();
        if (items->count() == 0 && stdIn()->isatty()) throw UsageError{"No input data"};

        if (encipher) {
            String password, retype;
            while (true) {
                password = Readline::getPassword("Enter password: ");
                String retype = Readline::getPassword("Enter again: ");
                if (password == retype) break;
                fout() << "Passwords differ, try again" << nl;
            }

            password = normalizePassword(password);

            File random{"/dev/random"};

            for (String path: items)
            {
                String contentKey = random->readSpan(16);
                String encipheredContentKey = String::allocate(contentKey->count());
                AesCipher{password}->encode(contentKey, mutate(encipheredContentKey));

                BlockCascade cipher{AesCipher{contentKey}};

                String fileName = path->fileName();
                String outPath = fileName + ".aes";
                if (File::exists(outPath)) {
                    ferr() << outPath << ": file exists, skipping" << nl;
                    continue;
                }
                Stream source = File{path};
                Stream sink;
                if (viewMode)
                    sink = stdOut();
                else
                    sink = File{outPath, FileOpen::Create|FileOpen::Truncate|FileOpen::WriteOnly};
                sink->write(encipheredContentKey);

                CipherSink cipherSink{cipher, sink, random};
                {
                    ByteSink header{cipherSink, mutate(String::allocate(0x100))};
                    String challenge = random->readSpan(16);
                    header->write(challenge);
                    header->writeUInt32(crc32(challenge));
                    header->writeInt32(fileName->count());
                    header->write(fileName);
                    header->writeUInt64(FileStatus{path}->size());
                }
                source->transferTo(cipherSink);

                File::unlink(path);
            }
        }
        else {
            SystemStream stdInSaved;
            SystemStream stdOutSaved;
            if (items->count() == 0) {
                items = StringList{""};
                stdInSaved->duplicate(stdIn());
                stdOutSaved->duplicate(stdOut());
                stdErr()->duplicateTo(stdIn());
                stdErr()->duplicateTo(stdOut());
            }

            String password = Readline::getPassword("Enter password: ");
            password = normalizePassword(password);

            if (viewMode) fout() << nl;

            for (String path: items)
            {
                Stream source;
                if (path == "")
                    source = stdInSaved;
                else
                    source = File{path};

                String contentKey = String::allocate(16);
                String encipheredContentKey = source->readSpan(16);
                AesCipher{password}->decode(encipheredContentKey, mutate(contentKey));

                BlockCascade cipher{AesCipher{contentKey}};
                CipherSource cipherSource{cipher, source};
                String origName;
                uint64_t origSize = 0;
                {
                    ByteSource header{cipherSource, mutate(String::allocate(1))};
                    String challenge = header->readSpan(16);
                    if (crc32(challenge) != header->readUInt32()) throw UsageError{"Invalid password or invalid file"};
                    int nameLength = header->readInt32();
                    origName = header->readSpan(nameLength);
                    origSize = header->readUInt64();

                    if (!viewMode && !filterMode) fout() << origName << nl;
                }
                Stream sink;
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
                    sink = File{origName, FileOpen::Create|FileOpen::Truncate|FileOpen::WriteOnly};
                }
                cipherSource->transferSpanTo(origSize, sink);
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
