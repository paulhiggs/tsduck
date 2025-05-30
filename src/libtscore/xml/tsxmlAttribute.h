//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2025, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//!
//!  @file
//!  Attribute of an XML element.
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsxmlTweaks.h"
#include "tsNames.h"
#include "tsIntegerUtils.h"
#include "tsTime.h"

namespace ts::xml {
    //!
    //! Attribute of an XML element.
    //! @ingroup libtscore xml
    //!
    class TSCOREDLL Attribute
    {
    public:
        //!
        //! Default constructor.
        //! The argument is initially invalid, everything will fail.
        //!
        Attribute();

        //!
        //! Full constructor.
        //! @param [in] name Attribute name with original case sensitivity.
        //! @param [in] value Attribute value.
        //! @param [in] line Line number in input document.
        //!
        explicit Attribute(const UString& name, const UString& value = UString(), size_t line = 0);

        //!
        //! Check if the attribute is valid.
        //! @return True if the attribute is valid.
        //!
        bool isValid() const { return _valid; }

        //!
        //! Get the line number in input document.
        //! @return The line number in input document, zero if the attribute was built programmatically.
        //!
        size_t lineNumber() const { return _line; }

        //!
        //! Get the attribute name with original case sensitivity.
        //! @return A constant reference to the attribute name with original case sensitivity.
        //!
        const UString& name() const { return _name; }

        //!
        //! Get the attribute value.
        //! @return A constant reference to the attribute value.
        //!
        const UString& value() const { return _value; }

        //!
        //! Get the formatted attribute value with quotes and escaped characters.
        //! @param [in] tweaks Formatting tweaks.
        //! @return The formatted value of the attribute.
        //!
        const UString formattedValue(const Tweaks& tweaks) const;

        //!
        //! Get the update sequence number.
        //! Each time an attribute is updated, a global (non-thread-safe) index is incremented.
        //! The method returns the value of the global index the last time the attribute was
        //! modified. This is a way to rebuild the list of attributes in their order of modification.
        //! @return The update sequence number.
        //!
        size_t sequence() const { return _sequence; }

        //!
        //! Set a string attribute.
        //! @param [in] value Attribute value.
        //!
        void setString(const UString& value);

        //!
        //! Set a bool attribute to a node.
        //! @param [in] value Attribute value.
        //!
        void setBool(bool value);

        //!
        //! Set an attribute with an integer value to a node.
        //! @tparam INT An integer type.
        //! @param [in] value Attribute value.
        //! @param [in] hexa If true, use an hexadecimal representation.
        //! When decimal is used, a comma is used as thousands separator.
        //! When hexadecimal is used, a 0x prefix is added.
        //!
        template <typename INT> requires std::integral<INT>
        void setInteger(INT value, bool hexa = false)
        {
            setString(hexa ? UString::Hexa(value) : UString::Decimal(value));
        }

        //!
        //! Set an enumeration attribute of a node.
        //! @tparam INT An integer or enum type.
        //! @param [in] definition The definition of enumeration values.
        //! @param [in] value Attribute value.
        //!
        template <typename INT> requires ts::int_enum<INT>
        void setEnum(const Names& definition, INT value)
        {
            setString(definition.name(value, true, 2 * sizeof(INT)));
        }

        //!
        //! Set an attribute with a floating point value to a node.
        //! @tparam FLT a floating point type.
        //! @param [in] value Attribute value.
        //! @param [in] width Width of the formatted number, not including the optional prefix and separator.
        //! @param [in] precision Precision to use after the decimal point.  Default is 6 digits.
        //! @param [in] force_sign If true, force a '+' sign for positive values.
        //!
        template <typename FLT> requires std::floating_point<FLT>
        void setFloat(FLT value, size_t width = 0, size_t precision = 6, bool force_sign = false)
        {
            setString(UString::Float(double(value), width, precision, force_sign));
        }

        //!
        //! Set a date/time attribute of an XML element.
        //! @param [in] value Attribute value.
        //!
        void setDateTime(const Time& value);

        //!
        //! Set a date (without hours) attribute of an XML element.
        //! @param [in] value Attribute value.
        //!
        void setDate(const Time& value);

        //!
        //! Set a time attribute of an XML element in "hh:mm:ss" format.
        //! @param [in] value Attribute value.
        //!
        template <class Rep, class Period>
        void setTime(const cn::duration<Rep,Period>& value)
        {
            setString(TimeToString(value));
        }

        //!
        //! Convert a date/time into a string, as required in attributes.
        //! @param [in] value Time value.
        //! @return The corresponding string.
        //!
        static UString DateTimeToString(const Time& value);

        //!
        //! Convert a date (without time) into a string, as required in attributes.
        //! @param [in] value Time value.
        //! @return The corresponding string.
        //!
        static UString DateToString(const Time& value);

        //!
        //! Convert a time (without date) into a string, as required in attributes.
        //! @param [in] value Time value.
        //! @return The corresponding string.
        //!
        template <class Rep, class Period>
        static UString TimeToString(const cn::duration<Rep,Period>& value);

        //!
        //! Convert a string into a date/time, as required in attributes.
        //! @param [in,out] value Time value. Unmodified in case of error.
        //! @param [in] str Time value as a string.
        //! @return True on success, false on error.
        //!
        static bool DateTimeFromString(Time& value, const UString& str);

        //!
        //! Convert a string into a date (without hours), as required in attributes.
        //! @param [in,out] value Date value. Unmodified in case of error.
        //! @param [in] str Date value as a string.
        //! @return True on success, false on error.
        //!
        static bool DateFromString(Time& value, const UString& str);

        //!
        //! Convert a string into a time, as required in attributes.
        //! @param [in,out] value Time value. Unmodified in case of error.
        //! @param [in] str Time value as a string.
        //! @return True on success, false on error.
        //!
        template <class Rep, class Period>
        static bool TimeFromString(cn::duration<Rep,Period>& value, const UString& str);

        //!
        //! Expand all environment variables in the attribute value.
        //! Environment variables are referenced using '${varname}' in text, attributes, names.
        //!
        void expandEnvironment();

        //!
        //! A constant static invalid instance.
        //! Used as universal invalid attribute.
        //! @return A constant reference to the universal invalid attribute.
        //!
        static const Attribute& INVALID();

    private:
        bool    _valid = false;
        UString _name {};
        UString _value {};
        size_t  _line = 0;
        size_t  _sequence = 0;  // insertion sequence

        // An for sequence numbers.
        static std::atomic_size_t _allocator;
    };
}


//----------------------------------------------------------------------------
// Template definitions.
//----------------------------------------------------------------------------

// Convert a time (without date) into a string, as required in attributes.
//! @cond nodoxygen
template <class Rep, class Period>
ts::UString ts::xml::Attribute::TimeToString(const cn::duration<Rep,Period>& value)
{
    const cn::seconds::rep sec = cn::duration_cast<cn::seconds>(value).count();
    return UString::Format(u"%02d:%02d:%02d", sec / 3600, (sec / 60) % 60, sec % 60);
}
//! @endcond

// Convert a string into a time, as required in attributes.
template <class Rep, class Period>
bool ts::xml::Attribute::TimeFromString(cn::duration<Rep,Period>& value, const UString& str)
{
    int hours = 0;
    int minutes = 0;
    int seconds = 0;
    const bool ok = str.scan(u"%d:%d:%d", &hours, &minutes, &seconds) &&
                    hours   >= 0 && hours   <= 23 &&
                    minutes >= 0 && minutes <= 59 &&
                    seconds >= 0 && seconds <= 59;
    if (ok) {
        value = cn::seconds((hours * 3600) + (minutes * 60) + seconds);
    }
    return ok;
}
