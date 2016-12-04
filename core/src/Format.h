/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>
#include <cc/Variant>
#include <cc/str>

namespace cc {

class Stream;
template<class T> class Queue;

/** \addtogroup formatted_io
  * \{
  */

/** \class FormatSignal Format.h <cc/Format>
  * \brief Type for formatting signals
  */
class FormatSignal {};

extern FormatSignal nl; ///< The newline formatting signal causes a newline but no output flushing.
extern FormatSignal flush; ///< The flush formatting signal causes the aggregated text to be flushed and written to the associated output stream (if any).

/** \class Format Format.h <cc/Format>
  * \brief Text formatting helper
  */
class Format: public Ref<StringList>
{
public:
    /** Construct a new string formatting helper
      * \param pattern formatting pattern containing injection placeholders ("%%")
      * \param stream output stream to write the resulting text to (optional)
      */
    Format(String pattern, Stream *stream = 0);

    /** Construct a new string formatting helper
      * \param stream output stream to write the resulting text to (optional)
      */
    Format(Stream *stream = 0);

    /// Low-level destructor
    ~Format();

    /// Copy-constructor
    Format(const Format &b);

    /// Assignment operator
    Format &operator=(const Format &b);

    /// Append or inject a string to the output text
    Format &operator<<(const String &s);

    /** %Process a special formatting signal
      * \param s formatting signal (nl or flush)
      */
    Format &operator<<(const FormatSignal &s);

    /** Append or inject an element x to the output text
      * \param x anything which can be converted to a string via str(x)
      */
    template<class T>
    inline Format &operator<<(const T &x) {
        if (isNull_) return *this;
        return *this << str(x);
    }

private:
    typedef Ref<StringList> Super;

    void flush();

    Ref<Stream> stream_;
    bool isNull_;
    Ref< Queue<int> > placeHolder_;
};

/** \class NullFormat Format.h <cc/Format>
  * \brief Zero storage formatting output sink
  */
class NullFormat {
public:
    /// Drop output
    template<class T>
    inline const NullFormat &operator<<(T) const { return *this; }
};

/** \}
  */

} // namespace cc
