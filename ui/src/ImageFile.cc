/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ImageFile>
#include <cc/ByteSink>
#include <cc/File>

namespace cc {

void ImageFile::write(const Stream &stream, const Image &image)
{
    const uint32_t preludeSize = 14;
    const uint32_t headerSizer = 108;
    const uint32_t dataOffset = preludeSize + headerSizer;

    ByteSink sink{stream, ByteOrder::LittleEndian};
    sink.write("BM");
    sink.writeUInt32(dataOffset + image.data().count()); // file size
    sink.writeUInt32(0); // reserved
    sink.writeUInt32(dataOffset); // offset of pixel data
    assert(sink.currentOffset() == preludeSize);
    sink.writeUInt32(headerSizer); // size of header
    sink.writeInt32(image.width()); // width in pixel
    sink.writeInt32(-image.height()); // height in pixel (negative means first line is top line)
    sink.writeUInt16(1); // number of color planes
    sink.writeUInt16(32); // bits per pixel
    sink.writeUInt32(3); // compression method (3 = none/bitfield)
    sink.writeUInt32(image.data().count()); // size of pixel data
    sink.writeUInt32(3937); // horizontal resolution (100 pixel/inch = 100/2.54 pixel/cm = 3937 pixel/m)
    sink.writeUInt32(3937); // vertical resolution
    sink.writeUInt32(0); // number of colors in the palette
    sink.writeUInt32(0); // number of important colors in color palette
    sink.setEndian(ByteOrder::BigEndian);
    sink.writeUInt32(0x00FF0000u); // red mask
    sink.writeUInt32(0x0000FF00u); // green mask
    sink.writeUInt32(0x000000FFu); // blue mask
    sink.writeUInt32(0xFF000000u); // alpha mask
    sink.setEndian(ByteOrder::LittleEndian);
    sink.write("Win ");
    for (int i = 0; i < 36; ++i) sink.writeUInt8(0); // not used (color space end points)
    sink.writeUInt32(0); // not used (red gamma)
    sink.writeUInt32(0); // not used (green gamma)
    sink.writeUInt32(0); // not used (blue gamma)
    assert(sink.currentOffset() == dataOffset);
    sink.write(image.data());
}

void ImageFile::save(const String &path, const Image &image)
{
    write(File{path, FileOpen::Overwrite}, image);
}

} // namespace cc
