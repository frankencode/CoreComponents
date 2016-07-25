/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Stream>
#include <cc/Exception>
#include <cc/SystemIo>
#include <cc/IoTarget>
#include <sys/socket.h> // SHUT_RD, etc.

namespace cc {

/** \class SystemStream SystemStream.h cc/SystemStream
  * \brief Base class for all system streams
  */
class SystemStream: public Stream, public IoTarget
{
public:
    /** Create a new SystemStream object
      * \param fd file descriptor
      * \return new object instance
      */
    static Ref<SystemStream> create(int fd) {
        return new SystemStream(fd);
    }

    /** Duplicate a system stream
      * \param other the system stream to duplicate
      * \return new object instance
      * The duplicate() method invokes ::dup(2) under its hood and creates a
      * corresponding new SystemStream instance for the newly created file descriptor.
      * \see duplicateTo()
      */
    static Ref<SystemStream> duplicate(SystemStream *other);

    /// The underlying file descriptor
    int fd() const;

    /** Return true if this is a TTY (i.e. a terminal device).
      *
      * This method is useful to distinguish between interactive from non-interactive
      * invocation.
      * ~~~~~~~~~~~~~
      * if (stdIn()->isatty()) fout() << "Hello, Jane!" << nl;
      * else; // continue silently
      * ~~~~~~~~~~~~~
      */
    bool isatty() const;

    virtual int read(ByteArray *data) override;
    virtual void write(const ByteArray *data) override;
    virtual void write(const StringList *parts) override;

    /// Convenience wrapper
    inline void write(const String &data) { write(data.get()); }

    /// Convenience wrapper
    inline void write(const char *data) { write(String(data)); }

    /// Convenience wrapper
    void write(const Format &data);

    /** Wait for a system I/O event
      * \param events combination of SystemIo::WaitEvent flags
      * \param interval_ms the interval to wait at max (-1 for infinite)
      * \return true if one of the indicated events occured, false on timeout
      */
    bool poll(int events, int interval_ms = -1);

    /// Full duplex connection shutdown
    enum ShutdownType {
        ReadShutdown = SHUT_RD,  ///< Partially shutdown the communication channel for reading
        WriteShutdown = SHUT_WR, ///< Partially shutdown the communication channel for writing
        FullShutdown = SHUT_RDWR ///< Fully shutdown the communication channel
    };

    /** Shutdown a full duplex communication channel
      * \param type shutdown for reading (ReadShutdown), writing (WriteShutdown) or both (FullShutdown)
      */
    void shutdown(ShutdownType type = FullShutdown);

    /// Currently set I/O scatter limit
    int scatterLimit() const { return scatterLimit_; }

    /** %Set the I/O scatter limit
      *
      * Scatter writes via write(const StringList *) and write(const Format &) will be merge into a single block
      * if the total size of the scatter list is below the scatter limit.
      */
    void setScatterLimit(int newLimit) { scatterLimit_ = newLimit; }

    /** Duplicate this file stream
      * \param other the other system stream to take over
      *
      * The duplicate() method invokes ::dup2(2) under its hood and thereby
      * duplicates this file stream onto the other file stream.
      *
      * This for instance allows to set a file as standard input:
      * ~~~~~~~~~~~~~
      * auto inSaved = stdIn()->duplicate();
      * File::open("binary.dat")->duplicateTo(stdIn());
      * ~~~~~~~~~~~~~
      * \see duplicate()
      */
    void duplicateTo(SystemStream *other);

protected:
    SystemStream(int fd = -1);
    ~SystemStream();

    int fd_;
    int scatterLimit_;
};

} // namespace cc
