//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2025, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//!
//!  @file
//!  TSDusk extensions repository
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsReport.h"
#include "tsLibTSDuckVersion.h"

namespace ts {
    //!
    //! A repository of TSDuck extensions.
    //! @ingroup libtsduck plugin
    //!
    //! This class is a singleton. Use static Instance() method to access the single instance.
    //!
    //! A TSDuck Extension is a dynamically loaded shared library. It is typically provided
    //! by some third party. Unlike tsp plugins, a TSDuck extension shared library has no
    //! callable interface. Instead, it statically registers hooks inside the TSDuck library,
    //! when the TSDuck extension shared library is loaded.
    //!
    //! The extensions repository is responsible for statically loading all available extensions.
    //! The extension shared libraries are all shared libraries named "tslibext_*", using the
    //! same search rules as tsp plugins.
    //!
    //! Since this operation occurs before entering the main code of any executable using the
    //! TSDuck library, the application has no control over the loading of extensions.
    //! The following environment variables can be defined to alter the loading of extensions:
    //!
    //! - TSLIBEXT_DEBUG : If defined and not empty, display debug messages on the standard error.
    //! - TSLIBEXT_NONE : If defined and not empty, do not load any extension.
    //! - TSLIBEXT_IGNORE : A comma-separated list of extensions to ignore (useful when one
    //!   extension creates problems when loaded).
    //!
    class TSDUCKDLL DuckExtensionRepository
    {
        TS_SINGLETON(DuckExtensionRepository);
    public:
        //!
        //! Get the number of loaded extensions.
        //! @return The number of loaded extensions.
        //!
        size_t extensionCount() const { return _extensions.size(); }

        //!
        //! List all loaded extensions.
        //! This function is typically used to implement the <code>tsversion -\-list-extensions</code> option.
        //! @param [in,out] report Where to report errors. Used to get verbosity level.
        //! @return The text to display.
        //!
        UString listExtensions(Report& report);

        //!
        //! A class to register extension.
        //!
        //! The registration is performed using constructors.
        //! Thus, it is possible to perform a registration in the declaration of a static object.
        //!
        class TSDUCKDLL Register
        {
            TS_NOBUILD_NOCOPY(Register);
        public:
            //!
            //! The constructor registers an extension.
            //! @param [in] name Extension name.
            //! @param [in] file_name Extension shared library file name. If empty, the caller's library is used.
            //! @param [in] description One-line description of the extension.
            //! @param [in] plugins List of tsp plugin names which are provided by this extension.
            //! @param [in] tools List of tools (executables) which are provided by this extension.
            //!
            Register(const UString& name,
                     const fs::path& file_name,
                     const UString& description,
                     const UStringVector& plugins = UStringVector(),
                     const UStringVector& tools = UStringVector());
        };

    private:
        TS_PUSH_WARNING()
        TS_MSC_NOWARNING(5027) // move assignment operator was implicitly defined as deleted
        // Description of one extension.
        class Extension
        {
        public:
            const UString       name;         // Extension name.
            const fs::path      file_name;    // Extension shared library file name.
            const UString       description;  // One-line description of the extension.
            const UStringVector plugins;      // List of tsp plugin names which are provided by this extension.
            const UStringVector tools;        // List of tools (executables) which are provided by this extension.
        };
        TS_POP_WARNING()

        // List of loaded extensions.
        std::list<Extension> _extensions {};

        // Load all extensions in constructor of an internal singleton.
        // There is a trick here. If we load all extensions in the constructor of DuckExtensionRepository,
        // the execution of the constructor of the singleton activates shared libraries which registers
        // themselves in the DuckExtensionRepository singletion (during its constructor). To void this,
        // an internal class singleton loads all extensions first.
        class Loader
        {
        public:
            Loader();
        };
        static const Loader LoaderInstance;
    };
}

//!
//! @hideinitializer
//! Export the TSDuck extension out of the shared library.
//! All TSDuck extension shared libraries must invoke this macro once.
//! The parameters are the same as the ts::DuckExtension constructor.
//!
//! Sample usage, from one source file inside the extension library:
//! @code
//! TS_REGISTER_EXTENSION(u"foo", u"Manipulate FOO tables", {u"fooinject", u"fooextract"}, {u"foogen"});
//! @endcode
//!
#define TS_REGISTER_EXTENSION(name,...) \
    TS_LIBTSDUCK_CHECK(); \
    static ts::DuckExtensionRepository::Register TS_UNIQUE_NAME(_RE)(name,ts::CallerLibraryFile(),__VA_ARGS__)
