/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_SYSTEMSTREAM_H
#define FLUX_SYSTEMSTREAM_H

#include <flux/Stream>
#include <flux/Exception>

namespace flux {

/** \brief System streams: files, character devices, stream sockets, etc.
  * \see File, SocketPair
  */
class SystemStream: public Stream
{
public:
    inline static Ref<SystemStream> create(int fd) {
        return new SystemStream(fd);
    }
    ~SystemStream();

    int fd() const;
    bool isatty() const;

    bool isOpen() const;
    void close();

    bool readyRead(double interval) const;

    virtual int read(ByteArray *data);
    virtual void write(const ByteArray *data);
    virtual void write(const StringList *parts);

    inline void write(Ref<ByteArray> data) { write(data.get()); }
    inline void write(String s) { write(s.get()); }
    inline void write(const char *s) { write(String(s)); }

    void closeOnExec();

    int ioctl(int request, void *arg);

protected:
    SystemStream(int fd, bool iov = true);

    int fd_;
    bool iov_;
    int iovMax_;
};

class ConnectionResetByPeer: public Exception
{
public:
    ~ConnectionResetByPeer() throw() {}

    virtual String message() const { return "Connection reset by peer"; }
};

} // namespace flux

#endif // FLUX_SYSTEMSTREAM_H
