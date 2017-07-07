/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Stream>

namespace cc {

/** \class HexDump HexDump.h cc/HexDump
  * \brief Hexdumping data sink
  *
  * Example usage:
  * ~~~~~~~~~~~~~
  * File::open("test.dat")->transferTo(HexDump::open(stdOut()));
  * ~~~~~~~~~~~~~
  *
  * \see StreamTap
  */
class HexDump: public Stream
{
public:
    /** Open a new hexdump sink
      * \param sink where the hexdump is written to (defaults to stdOut())
      * \return new object instance
      */
    static Ref<HexDump> open(Stream *sink = 0) { return new HexDump(sink); }

    void write(const ByteArray *data) override;

private:
    HexDump(Stream *sink);
    ~HexDump();

    void writeByte(uint8_t ch);
    void flush();

    Ref<Stream> sink_;
    off_t offset_;
    Ref<ByteArray> buffer_;
    int i_;
};

/** Convert binary data into a hexdump
  * \param data binary data
  * \return hexdump
  */
String hexDump(const ByteArray *data);

} // namespace cc
