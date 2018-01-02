//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2018, Thierry Lelegard
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------
//!
//!  @file
//!  Send output to a "pager" application such as "more".
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsForkPipe.h"

namespace ts {
    //!
    //! Send application output to a "pager" application such as "more" or "less".
    //!
    class TSDUCKDLL OutputPager : public ForkPipe
    {
    public:
        //!
        //! Default constructor.
        //! @param [in] envName Name of the optional environment variable containing the pager command name.
        //!
        OutputPager(const UString& envName = u"PAGER");

        //!
        //! Check if we can run a pager.
        //! To run a pager, we must have found a valid pager command and either stdout or stderr must be a terminal.
        //! @return True if we can page.
        //!
        bool canPage() const { return _hasTerminal && !_pagerCommand.empty(); }

        //!
        //! Get the pager command which is used.
        //! @return The pager command which is used.
        //!
        UString pagerCommand() const { return _pagerCommand; }

        //!
        //! Create the process, open the pipe.
        //! @param [in] synchronous If true, wait for process termination in close().
        //! @param [in] buffer_size The pipe buffer size in bytes. Used on Windows only. Zero means default.
        //! @param [in,out] report Where to report errors.
        //! @return True on success, false on error.
        //!
        bool open(bool synchronous, size_t buffer_size, Report& report);

        //!
        //! Write data to the pipe (received at process' standard input).
        //! @param [in] test Text to write.
        //! @param [in,out] report Where to report errors.
        //! @return True on success, false on error.
        //!
        bool write(const UString& text, Report& report);

    private:
        bool       _hasTerminal;
        OutputMode _outputMode;
        UString    _pagerCommand;

        // Inacessible operations.
        OutputPager(const OutputPager&) = delete;
        OutputPager& operator=(const OutputPager&) = delete;
    };
}
