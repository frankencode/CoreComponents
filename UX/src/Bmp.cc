/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Bmp>
#include <cc/ByteSink>
#include <cc/ByteSource>
#include <cc/File>

namespace cc {

bool Bmp::detect(const Bytes &data, Out<int> width, Out<int> height)
{
    bool ok = true;
    try {
        readHeader(ByteSource{data, ByteOrder::LittleEndian}, &width, &height);
    }
    catch (BmpError &) {
        ok = false;
    }
    return ok;
}

Image Bmp::map(const String &path)
{
    File file{path, FileOpen::ReadOnly};
    ByteSource source{file, ByteOrder::LittleEndian};

    int width = 0, height = 0;
    uint32_t fileSize = 0, dataOffset = 0;
    readHeader(source, &width, &height, &fileSize, &dataOffset);

    return Image{width, -height, file.map(dataOffset, fileSize)};
}

Image Bmp::decode(const Bytes &data)
{
    int width = 0, height = 0;
    uint32_t fileSize = 0, dataOffset = 0;

    try {
        ByteSource source{data, ByteOrder::LittleEndian};
        readHeader(source, &width, &height, &fileSize, &dataOffset);
    }
    catch (BmpError &) {
        return Image{};
    }

    Bytes pixelData = data;;
    pixelData.shift(dataOffset);

    return Image{width, -height, pixelData};
}

void Bmp::encode(const Stream &stream, const Image &image)
{
    const uint32_t preludeSize = 14;
    const uint32_t headerSize = 108;
    const uint32_t dataOffset = preludeSize + headerSize;

    ByteSink sink{stream, ByteOrder::LittleEndian};
    sink.write("BM");
    sink.writeUInt32(dataOffset + image.data().count()); // file size
    sink.writeUInt32(0); // reserved
    sink.writeUInt32(dataOffset); // offset of pixel data
    assert(sink.currentOffset() == preludeSize);
    sink.writeUInt32(headerSize); // size of header
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
    if (localEndian() == ByteOrder::LittleEndian) {
        /** Because the image data is copied out of memory and is always in local endian ARGB
          * we use big endian for the bitfield masks only on little endian systems. Thereby the byte order
          * of the image data automatically fits to the masks although we do not explicity map the byte oder
          * of the image pixels. This allows image files generated on big endian system to be directly memory mapped
          * on big endian systems and image files generated on little endian systems to be directly memory mapped on
          * little endian systems. In any case a correct BMP file is generated, readable with any external tool
          * supporting the Windows BMP file format.
          */
        sink.setEndian(ByteOrder::BigEndian);
    }
    sink.writeUInt32(0x0000FF00u); // red mask
    sink.writeUInt32(0x00FF0000u); // green mask
    sink.writeUInt32(0xFF000000u); // blue mask
    sink.writeUInt32(0x000000FFu); // alpha mask
    if (sink.endian() != ByteOrder::LittleEndian) {
        sink.setEndian(ByteOrder::LittleEndian);
    }
    sink.write("Win ");
    for (int i = 0; i < 36; ++i) sink.writeUInt8(0); // not used (color space end points)
    sink.writeUInt32(0); // not used (red gamma)
    sink.writeUInt32(0); // not used (green gamma)
    sink.writeUInt32(0); // not used (blue gamma)
    assert(sink.currentOffset() == dataOffset);
    sink.write(image.data());
}

void Bmp::save(const String &path, const Image &image)
{
    encode(File{path, FileOpen::Overwrite}, image);
}

void Bmp::readHeader(ByteSource source, Out<int> width, Out<int> height, Out<uint32_t> fileSize, Out<uint32_t> dataOffset)
{
    char bm[2];
    source.readChar(&bm[0]);
    source.readChar(&bm[1]);
    if (bm[0] != 'B' || bm[1] != 'M') throw BmpError{"not a BMP file"};
    fileSize = source.readUInt32();
    source.readUInt32();
    dataOffset = source.readUInt32();
    /*const uint32_t headerSize =*/ source.readUInt32();
    width = source.readInt32();
    height = source.readInt32();
    if (width() <= 0 || height() >= 0) throw BmpError{"unsupported image orientation"};
    source.readUInt16(); // number of planes
    if (source.readUInt16() != 32) throw BmpError{"not a 32-bit ARGB image"};
    if (source.readUInt32() != 3) throw BmpError{"unsupported compression method"};
    /*const uint32_t dataSize =*/ source.readUInt32();
    for (int i = 0; i < 4; ++i) source.readUInt32();

    const uint32_t redMask = source.readUInt32();
    const uint32_t greenMask = source.readUInt32();
    const uint32_t blueMask = source.readUInt32();
    const uint32_t alphaMask = source.readUInt32();
    const ByteOrder targetEndian = localEndian();

    if (!(
        (
            targetEndian == ByteOrder::BigEndian &&
            redMask   == 0x0000FF00u &&
            greenMask == 0x00FF0000u &&
            blueMask  == 0xFF000000u &&
            alphaMask == 0x000000FFu
        ) ||
        (
            targetEndian == ByteOrder::LittleEndian &&
            redMask   == 0x00FF0000u &&
            greenMask == 0x0000FF00u &&
            blueMask  == 0x000000FFu &&
            alphaMask == 0xFF000000u
        )
    )) {
       throw BmpError{"image data not mapable"};
    }
}

BmpError::BmpError(const char *reason):
    reason_{reason}
{}

String BmpError::message() const
{
    return Format{"Unexpected image file format: %%"}.arg(reason_);
}

} // namespace cc
