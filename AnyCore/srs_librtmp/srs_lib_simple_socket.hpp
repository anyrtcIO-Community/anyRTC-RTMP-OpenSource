/*
The MIT License (MIT)

Copyright (c) 2013-2015 SRS(ossrs)

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef SRS_LIB_SIMPLE_SOCKET_HPP
#define SRS_LIB_SIMPLE_SOCKET_HPP

/*
#include <srs_lib_simple_socket.hpp>
*/

#include "srs_librtmp.h"
#include "srs_rtmp_io.hpp"

// for srs-librtmp, @see https://github.com/ossrs/srs/issues/213
#ifndef _WIN32
    #define SOCKET int
#endif

#ifndef _WIN32
#include <sys/uio.h>
#endif

/**
* simple socket stream,
* use tcp socket, sync block mode, for client like srs-librtmp.
*/
class SimpleSocketStream : public ISrsProtocolReaderWriter
{
public:
	SimpleSocketStream(){};
	virtual ~SimpleSocketStream(){};
public:
    virtual srs_hijack_io_t hijack_io() = 0;
	virtual int create_socket() = 0;
	virtual int connect(const char* server, int port) = 0;
	virtual int disconnect() = 0;
// ISrsBufferReader
public:
	virtual int read(void* buf, size_t size, ssize_t* nread) = 0;
// ISrsProtocolReader
public:
	virtual void set_recv_timeout(int64_t timeout_us) = 0;
	virtual int64_t get_recv_timeout() = 0;
	virtual int64_t get_recv_bytes() = 0;
// ISrsProtocolWriter
public:
	virtual void set_send_timeout(int64_t timeout_us) = 0;
	virtual int64_t get_send_timeout() = 0;
	virtual int64_t get_send_bytes() = 0;
	virtual int writev(const iovec *iov, int iov_size, ssize_t* nwrite) = 0;
// ISrsProtocolReaderWriter
public:
	virtual bool is_never_timeout(int64_t timeout_us) = 0;
	virtual int read_fully(void* buf, size_t size, ssize_t* nread) = 0;
	virtual int write(void* buf, size_t size, ssize_t* nwrite) = 0;
};

#endif

