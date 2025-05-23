//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2025, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//!
//!  @file
//!  Digital TV tuner.
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsTunerBase.h"
#include "tsModulationArgs.h"
#include "tsAbortInterface.h"

namespace ts {
    //!
    //! General-purpose implementation of a digital TV tuner.
    //! @ingroup libtsduck hardware
    //!
    //! This class encapsulates physical tuners and file-based tuner emulators.
    //! When a "tuner name" is an XML file (a file path ending in ".xml"), the
    //! tuner emulator is used. Otherwise, a physical tuner is used.
    //!
    //! The syntax of a physical tuner "device name" depends on the operating system.
    //!
    //! Linux:
    //! - Syntax: /dev/dvb/adapterA[:F[:M[:V]]]
    //! - A = adapter number
    //! - F = frontend number (default: 0)
    //! - M = demux number (default: 0)
    //! - V = dvr number (default: 0)
    //!
    //! Windows:
    //! - DirectShow/BDA tuner filter name
    //!
    class TSDUCKDLL Tuner: public TunerBase
    {
        TS_NOBUILD_NOCOPY(Tuner);
    public:
        //!
        //! Constructor.
        //! @param [in,out] duck TSDuck execution context.
        //!
        Tuner(DuckContext& duck);

        //!
        //! Destructor.
        //!
        virtual ~Tuner() override;

        //!
        //! Constructor and open device name.
        //! @param [in,out] duck TSDuck execution context.
        //! @param [in] device_name Tuner device name. If the name is empty, use the "first" or "default" tuner.
        //! If the name is a file path ending in ".xml", a tuner emulator is used.
        //! @param [in] info_only If true, we will only fetch the properties of
        //! the tuner, we won't use it to receive streams. Thus, it is possible
        //! to open tuners which are already used to actually receive a stream.
        //!
        Tuner(DuckContext& duck, const UString& device_name, bool info_only);

        // Implementation of TunerBase.
        virtual bool open(const UString& device_name, bool info_only) override;
        virtual bool close(bool silent = false) override;
        virtual bool isOpen() const override;
        virtual bool infoOnly() const override;
        virtual const DeliverySystemSet& deliverySystems() const override;
        virtual UString deviceName() const override;
        virtual UString deviceInfo() const override;
        virtual UString devicePath() const override;
        virtual bool getSignalState(SignalState& state) override;
        virtual bool tune(ModulationArgs& params) override;
        virtual bool start() override;
        virtual bool stop(bool silent = false) override;
        virtual void abort(bool silent = false) override;
        virtual size_t receive(TSPacket* buffer, size_t max_packets, const AbortInterface* abort = nullptr) override;
        virtual bool getCurrentTuning(ModulationArgs& params, bool reset_unknown) override;
        virtual void setSignalTimeout(cn::milliseconds t) override;
        virtual void setSignalTimeoutSilent(bool silent) override;
        virtual bool setReceiveTimeout(cn::milliseconds t) override;
        virtual cn::milliseconds receiveTimeout() const override;
        virtual void setSignalPoll(cn::milliseconds t) override;
        virtual void setDemuxBufferSize(size_t s) override;
        virtual void setSinkQueueSize(size_t s) override;
        virtual void setReceiverFilterName(const UString& name) override;
        virtual std::ostream& displayStatus(std::ostream& strm, const UString& margin = UString(), bool extended = false) override;

    private:
        TunerBase* _device = nullptr;    // Physical tuner device.
        TunerBase* _emulator = nullptr;  // File-based tuner emulator.
        TunerBase* _current = nullptr;   // Current tuner.
    };
}
