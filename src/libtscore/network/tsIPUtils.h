//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2025, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//!
//!  @file
//!  @ingroup libtscore net
//!  Utilities for IP networking
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsCerrReport.h"
#include "tsIP.h"

namespace ts {
    //!
    //! Initialize the IP libraries in the current process.
    //! On some systems (UNIX), there is no need to initialize IP.
    //! On other systems (Windows), using IP and socket without initialization fails.
    //! This method is a portable way to ensure that IP is properly initialized.
    //! It shall be called at least once before using IP in the application.
    //! @ingroup net
    //! @return True on success, false on error.
    //!
    TSCOREDLL bool IPInitialize(Report& = CERR);

    //!
    //! Get the std::error_category for getaddrinfo() error code (Unix only).
    //! @ingroup net
    //! @return A constant reference to a std::error_category instance.
    //!
    TSCOREDLL const std::error_category& getaddrinfo_category();

    //
    // The rest of this file contains portable definitions for the system socket interface.
    // Most socket types and functions have identical API in UNIX and Windows.
    // However, there are some slight incompatibilities which are solved using the following definitions.
    //

    //!
    //! Data type for socket descriptors as returned by the socket() system call.
    //! @ingroup net
    //!
#if defined(DOXYGEN)
    using SysSocketType = platform_specific;
#elif defined(TS_WINDOWS)
    using SysSocketType = ::SOCKET;
#elif defined(TS_UNIX)
    using SysSocketType = int;
#endif

    //!
    //! Value of type SysSocketType which is returned by the socket() system call in case of failure.
    //! @ingroup net
    //! Example:
    //! @code
    //! SysSocketType sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    //! if (sock == SYS_SOCKET_INVALID) {
    //!     ... error processing ...
    //! }
    //! @endcode
    //!
#if defined(DOXYGEN)
    constexpr SysSocketType SYS_SOCKET_INVALID = platform_specific;
#elif defined(TS_WINDOWS)
    constexpr SysSocketType SYS_SOCKET_INVALID = INVALID_SOCKET;
#elif defined(TS_UNIX)
    constexpr SysSocketType SYS_SOCKET_INVALID = -1;
#endif

    //!
    //! System error code value meaning "connection reset by peer".
    //! @ingroup net
    //!
#if defined(DOXYGEN)
    constexpr int SYS_SOCKET_ERR_RESET = platform_specific;
#elif defined(TS_WINDOWS)
    constexpr int SYS_SOCKET_ERR_RESET = WSAECONNRESET;
#elif defined(TS_UNIX)
    constexpr int SYS_SOCKET_ERR_RESET = EPIPE;
#endif

    //!
    //! System error code value meaning "peer socket not connected".
    //! @ingroup net
    //!
#if defined(DOXYGEN)
    constexpr int SYS_SOCKET_ERR_NOTCONN = platform_specific;
#elif defined(TS_WINDOWS)
    constexpr int SYS_SOCKET_ERR_NOTCONN = WSAENOTCONN;
#elif defined(TS_UNIX)
    constexpr int SYS_SOCKET_ERR_NOTCONN = ENOTCONN;
#endif

    //!
    //! Integer data type which receives the length of a struct sockaddr.
    //! @ingroup net
    //! Example:
    //! @code
    //! struct sockaddr sock_addr;
    //! SysSocketLengthType len = sizeof(sock_addr);
    //! if (getsockname(sock, &sock_addr, &len) != 0) {
    //!     ... error processing ...
    //! }
    //! @endcode
    //!
#if defined(DOXYGEN)
    using SysSocketLengthType = platform_specific;
#elif defined(TS_WINDOWS)
    using SysSocketLengthType = int;
#elif defined(TS_UNIX)
    using SysSocketLengthType = ::socklen_t;
#endif

    //!
    //! Integer data type for a "signed size" returned from send() or recv() system calls.
    //! @ingroup net
    //! Example:
    //! @code
    //! SysSocketSignedSizeType got = recv(sock, SysRecvBufferPointer(&data), max_size, 0);
    //! @endcode
    //!
#if defined(DOXYGEN)
    using SysSocketSignedSizeType = platform_specific;
#elif defined(TS_WINDOWS)
    using SysSocketSignedSizeType = int;
#elif defined(TS_UNIX)
    using SysSocketSignedSizeType = ::ssize_t;
#endif

    //!
    //! Integer data type for the Time To Live (TTL) socket option.
    //! @ingroup net
    //! Example:
    //! @code
    //! SysSocketTTLType ttl = 10;
    //! if (setsockopt(sock, IPPROTO_IP, IP_TTL, SysSockOptPointer(&ttl), sizeof(ttl)) != 0) {
    //!     ... error processing ...
    //! }
    //! @endcode
    //!
#if defined(DOXYGEN)
    using SysSocketTTLType = platform_specific;
#elif defined(TS_WINDOWS)
    using SysSocketTTLType = ::DWORD;
#elif defined(TS_UNIX)
    using SysSocketTTLType = int;
#endif

    //!
    //! Integer data type for the multicast Time To Live (TTL) socket option.
    //! @ingroup net
    //! Example:
    //! @code
    //! SysSocketMulticastTTLType mttl = 1;
    //! if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, SysSockOptPointer(&mttl), sizeof(mttl)) != 0) {
    //!     ... error processing ...
    //! }
    //! @endcode
    //!
#if defined(DOXYGEN)
    using SysSocketMulticastTTLType = platform_specific;
#elif defined(TS_WINDOWS)
    using SysSocketMulticastTTLType = ::DWORD;
#elif defined(TS_UNIX)
    using SysSocketMulticastTTLType = unsigned char;
#endif

    //!
    //! Integer data type for the Type Of Service (TOS) IPv4 socket option.
    //! @ingroup net
    //!
#if defined(DOXYGEN)
    using SysSocketTOSType = platform_specific;
#elif defined(TS_WINDOWS)
    using SysSocketTOSType = ::DWORD;
#elif defined(TS_UNIX)
    using SysSocketTOSType = int;
#endif

    //!
    //! Integer data type for the Traffic Class (TCLASS) IPv6 socket option.
    //! @ingroup net
    //!
#if defined(DOXYGEN)
    using SysSocketTClassType = platform_specific;
#elif defined(TS_WINDOWS)
    using SysSocketTClassType = ::DWORD;
#elif defined(TS_UNIX)
    using SysSocketTClassType = int;
#endif

    //!
    //! Integer data type for the IPV6_V6ONLY socket option.
    //! @ingroup net
    //!
#if defined(DOXYGEN)
    using SysSocketV6OnlyType = platform_specific;
#elif defined(TS_WINDOWS)
    using SysSocketV6OnlyType = ::DWORD;
#elif defined(TS_UNIX)
    using SysSocketV6OnlyType = int;
#endif

    //!
    //! Integer data type for the IPv4 multicast loop socket option.
    //! @ingroup net
    //! Example:
    //! @code
    //! SysSocketMulticastLoopType mloop = 1;
    //! if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP, SysSockOptPointer(&mloop), sizeof(mloop)) != 0) {
    //!     ... error processing ...
    //! }
    //! @endcode
    //!
#if defined(DOXYGEN)
    using SysSocketMulticastLoopType = platform_specific;
#elif defined(TS_WINDOWS)
    using SysSocketMulticastLoopType = ::DWORD;
#elif defined(TS_UNIX)
    using SysSocketMulticastLoopType = unsigned char;
#endif

    //!
    //! Integer data type for the IPv6 multicast loop socket option.
    //! @ingroup net
    //! Example:
    //! @code
    //! SysSocketMulticastLoopType6 mloop = 1;
    //! if (setsockopt(sock, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, SysSockOptPointer(&mloop), sizeof(mloop)) != 0) {
    //!     ... error processing ...
    //! }
    //! @endcode
    //!
#if defined(DOXYGEN)
    using SysSocketMulticastLoopType6 = platform_specific;
#elif defined(TS_WINDOWS)
    using SysSocketMulticastLoopType6 = ::DWORD;
#elif defined(TS_UNIX)
    using SysSocketMulticastLoopType6 = unsigned int;
#endif

    //!
    //! Integer data type for the IP_PKTINFO socket option.
    //! @ingroup net
    //! Example:
    //! @code
    //! SysSocketPktInfoType state = 1;
    //! if (setsockopt(sock, IPPROTO_IP, IP_PKTINFO, SysSockOptPointer(&state), sizeof(state)) != 0) {
    //!     ... error processing ...
    //! }
    //! @endcode
    //!
#if defined(DOXYGEN)
    using SysSocketPktInfoType = platform_specific;
#elif defined(TS_WINDOWS)
    using SysSocketPktInfoType = ::DWORD;
#elif defined(TS_UNIX)
    using SysSocketPktInfoType = int;
#endif

    //!
    //! Integer data type for the field l_linger in the struct linger socket option.
    //! @ingroup net
    //! All systems do not use the same type size and this may generate some warnings.
    //! Example:
    //! @code
    //! struct linger lin;
    //! lin.l_linger = SysSocketLingerType(seconds);
    //! @endcode
    //!
#if defined(DOXYGEN)
    using SysSocketLingerType = platform_specific;
#elif defined(TS_WINDOWS)
    using SysSocketLingerType = u_short;
#elif defined(TS_UNIX)
    using SysSocketLingerType = int;
#endif

    //!
    //! Pointer type for the address of a socket option value.
    //! @ingroup net
    //! The "standard" parameter type is @c void* but some systems use other exotic values.
    //! Example:
    //! @code
    //! SysSocketTTLType ttl = 10;
    //! if (setsockopt(sock, IPPROTO_IP, IP_TTL, SysSockOptPointer(&ttl), sizeof(ttl)) != 0) {
    //!     ... error processing ...
    //! }
    //! @endcode
    //!
#if defined(DOXYGEN)
    using SysSockOptPointer = platform_specific;
#elif defined(TS_WINDOWS)
    using SysSockOptPointer = const char*;
#elif defined(TS_UNIX)
    using SysSockOptPointer = void*;
#endif

    //!
    //! Pointer type for the address of the data buffer for a recv() system call.
    //! @ingroup net
    //! The "standard" parameter type is @c void* but some systems use other exotic values.
    //! Example:
    //! @code
    //! SysSocketSignedSizeType got = recv(sock, SysRecvBufferPointer(&data), max_size, 0);
    //! @endcode
    //!
#if defined(DOXYGEN)
    using SysRecvBufferPointer = platform_specific;
#elif defined(TS_WINDOWS)
    using SysRecvBufferPointer = char*;
#elif defined(TS_UNIX)
    using SysRecvBufferPointer = void*;
#endif

    //!
    //! Pointer type for the address of the data buffer for a send() system call.
    //! @ingroup net
    //! The "standard" parameter type is @c void* but some systems use other exotic values.
    //! Example:
    //! @code
    //! SysSocketSignedSizeType gone = send(sock, SysSendBufferPointer(&data), SysSendSizeType(size), 0);
    //! @endcode
    //!
#if defined(DOXYGEN)
    using SysSendBufferPointer = platform_specific;
#elif defined(TS_WINDOWS)
    using SysSendBufferPointer = const char*;
#elif defined(TS_UNIX)
    using SysSendBufferPointer = const void*;
#endif

    //!
    //! Integer type for the size of the data buffer for a send() system call.
    //! @ingroup net
    //!
#if defined(DOXYGEN)
    using SysSendSizeType = platform_specific;
#elif defined(TS_WINDOWS)
    using SysSendSizeType = int;
#elif defined(TS_UNIX)
    using SysSendSizeType = size_t;
#endif

    //!
    //! Name of the option for the shutdown() system call which means "close on both directions".
    //! @ingroup net
    //! Example:
    //! @code
    //! shutdown(sock, SYS_SOCKET_SHUT_RDWR);
    //! @endcode
    //!
#if defined(DOXYGEN)
    constexpr int SYS_SOCKET_SHUT_RDWR = platform_specific;
#elif defined(TS_WINDOWS)
    constexpr int SYS_SOCKET_SHUT_RDWR = SD_BOTH;
#elif defined(TS_UNIX)
    constexpr int SYS_SOCKET_SHUT_RDWR = SHUT_RDWR;
#endif

    //!
    //! Name of the option for the shutdown() system call which means "close on receive side".
    //! @ingroup net
    //! Example:
    //! @code
    //! shutdown(sock, SYS_SOCKET_SHUT_RD);
    //! @endcode
    //!
#if defined(DOXYGEN)
    constexpr int SYS_SOCKET_SHUT_RD = platform_specific;
#elif defined(TS_WINDOWS)
    constexpr int SYS_SOCKET_SHUT_RD = SD_RECEIVE;
#elif defined(TS_UNIX)
    constexpr int SYS_SOCKET_SHUT_RD = SHUT_RD;
#endif

    //!
    //! Name of the option for the shutdown() system call which means "close on send side".
    //! @ingroup net
    //! Example:
    //! @code
    //! shutdown(sock, SYS_SOCKET_SHUT_WR);
    //! @endcode
    //!
#if defined(DOXYGEN)
    constexpr int SYS_SOCKET_SHUT_WR = platform_specific;
#elif defined(TS_WINDOWS)
    constexpr int SYS_SOCKET_SHUT_WR = SD_SEND;
#elif defined(TS_UNIX)
    constexpr int SYS_SOCKET_SHUT_WR = SHUT_WR;
#endif

    //!
    //! The close() system call which applies to socket devices.
    //! The "standard" name is @c close but some systems use other exotic names.
    //! @ingroup net
    //! @param [in] sock System socket descriptor.
    //! @return Error code.
    //!
    TSCOREDLL inline int SysCloseSocket(SysSocketType sock)
    {
#if defined(TS_WINDOWS)
        return ::closesocket(sock);
#elif defined(TS_UNIX)
        return ::close(sock);
#else
        #error "Unsupported operating system"
#endif
    }
}
