#pragma once

#include <cc/Stream>

namespace cc {

/** \class ReplaySource ReplaySource.h cc/ReplaySource
  * \ingroup binary_io
  * \brief Feed in data from an in-memory buffer
  * \see CaptureSink, ByteSource
  */
class ReplaySource: public Stream
{
public:
    /** Open a new ReplaySource
      * \param buffer input buffer
      * \return new object instance
      */
    static Ref<ReplaySource> open(const ByteArray *buffer) { return new ReplaySource(buffer); }

    /// The input buffer used
    const ByteArray *buffer() const { return buffer_; }

    int read(ByteArray *data) override;

private:
    ReplaySource(const ByteArray *buffer);

    Ref<const ByteArray> buffer_;
    int i_;
};

} // namespace cc {
