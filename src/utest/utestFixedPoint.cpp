//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2025, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//
//  TSUnit test suite for FixedPoint class.
//
//----------------------------------------------------------------------------

#include "tsFixedPoint.h"
#include "tsunit.h"


//----------------------------------------------------------------------------
// The test fixture
//----------------------------------------------------------------------------

class FixedPointTest: public tsunit::Test
{
    TSUNIT_DECLARE_TEST(Unit);
    TSUNIT_DECLARE_TEST(SubUnit);
    TSUNIT_DECLARE_TEST(Assignment);
    TSUNIT_DECLARE_TEST(Comparison);
    TSUNIT_DECLARE_TEST(Bounds);
    TSUNIT_DECLARE_TEST(Overflow);
    TSUNIT_DECLARE_TEST(ToString);
    TSUNIT_DECLARE_TEST(FromString);
};

TSUNIT_REGISTER(FixedPointTest);


//----------------------------------------------------------------------------
// Unitary tests.
//----------------------------------------------------------------------------

TSUNIT_DEFINE_TEST(Unit)
{
    using Fixed = ts::FixedPoint<int32_t, 0>;

    Fixed i;
    Fixed n(-3);
    Fixed z(0);
    Fixed p(47);

    TSUNIT_EQUAL(0, Fixed::PRECISION);
    TSUNIT_EQUAL(1, Fixed::FACTOR);
    TSUNIT_EQUAL(-3, n.toInt());
    TSUNIT_EQUAL(0, z.toInt());
    TSUNIT_EQUAL(47, p.toInt());
    TSUNIT_EQUAL(-3, n.raw());
    TSUNIT_EQUAL(0, z.raw());
    TSUNIT_EQUAL(47, p.raw());
    TSUNIT_EQUAL(21, Fixed(21, false).toInt());
    TSUNIT_EQUAL(21, Fixed(21, true).toInt());

    TSUNIT_EQUAL(7, (-Fixed(-7)).toInt());
    TSUNIT_EQUAL(-7, (-Fixed(7)).toInt());

    TSUNIT_EQUAL(23, (Fixed(21) + Fixed(2)).toInt());
    TSUNIT_EQUAL(30, (Fixed(21) + 9).toInt());
    TSUNIT_EQUAL(19, (Fixed(21) - Fixed(2)).toInt());
    TSUNIT_EQUAL(12, (Fixed(21) - 9).toInt());
    TSUNIT_EQUAL(42, (Fixed(21) * 2).toInt());
    TSUNIT_EQUAL(10, (Fixed(21) / 2).toInt());

    TSUNIT_EQUAL(30, (9 + Fixed(21)).toInt());
    TSUNIT_EQUAL(-12, (9 - Fixed(21)).toInt());
    TSUNIT_EQUAL(42, (2 * Fixed(21)).toInt());

    TSUNIT_EQUAL(21, (Fixed(3) * Fixed(7)).toInt());
    TSUNIT_EQUAL(21, (3 * Fixed(7)).toInt());
    TSUNIT_EQUAL(21, (Fixed(3) * 7).toInt());

    TSUNIT_EQUAL(2, (Fixed(10) / Fixed(4)).toInt());
    TSUNIT_EQUAL(2, (Fixed(10) / Fixed(4)).raw());
    TSUNIT_EQUAL(2, (10 / Fixed(4)).toInt());
    TSUNIT_EQUAL(2, (10 / Fixed(4)).raw());
    TSUNIT_EQUAL(2, (Fixed(10) / 4).toInt());
    TSUNIT_EQUAL(2, (Fixed(10) / 4).raw());
}

TSUNIT_DEFINE_TEST(SubUnit)
{
    using Fixed = ts::FixedPoint<int32_t, 3>;

    Fixed i;
    Fixed n(-3);
    Fixed z(0);
    Fixed p(47);

    TSUNIT_EQUAL(3, Fixed::PRECISION);
    TSUNIT_EQUAL(1000, Fixed::FACTOR);
    TSUNIT_EQUAL(-3, n.toInt());
    TSUNIT_EQUAL(0, z.toInt());
    TSUNIT_EQUAL(47, p.toInt());
    TSUNIT_EQUAL(-3000, n.raw());
    TSUNIT_EQUAL(0, z.raw());
    TSUNIT_EQUAL(47000, p.raw());
    TSUNIT_EQUAL(21, Fixed(21, false).toInt());
    TSUNIT_EQUAL(21, Fixed(21234, true).toInt());

    TSUNIT_EQUAL(7, (-Fixed(-7)).toInt());
    TSUNIT_EQUAL(-7, (-Fixed(7)).toInt());

    TSUNIT_EQUAL(23, (Fixed(21) + Fixed(2)).toInt());
    TSUNIT_EQUAL(30, (Fixed(21) + 9).toInt());
    TSUNIT_EQUAL(19, (Fixed(21) - Fixed(2)).toInt());
    TSUNIT_EQUAL(12, (Fixed(21) - 9).toInt());
    TSUNIT_EQUAL(42, (Fixed(21) * 2).toInt());
    TSUNIT_EQUAL(10, (Fixed(21) / 2).toInt());

    TSUNIT_EQUAL(30, (9 + Fixed(21)).toInt());
    TSUNIT_EQUAL(-12, (9 - Fixed(21)).toInt());
    TSUNIT_EQUAL(42, (2 * Fixed(21)).toInt());

    TSUNIT_EQUAL(21, (Fixed(3) * Fixed(7)).toInt());
    TSUNIT_EQUAL(21, (3 * Fixed(7)).toInt());
    TSUNIT_EQUAL(21, (Fixed(3) * 7).toInt());

    TSUNIT_EQUAL(2, (Fixed(10) / Fixed(4)).toInt());
    TSUNIT_EQUAL(2500, (Fixed(10) / Fixed(4)).raw());
    TSUNIT_EQUAL(2, (10 / Fixed(4)).toInt());
    TSUNIT_EQUAL(2500, (10 / Fixed(4)).raw());
    TSUNIT_EQUAL(2, (Fixed(10) / 4).toInt());
    TSUNIT_EQUAL(2500, (Fixed(10) / 4).raw());
}

TSUNIT_DEFINE_TEST(Assignment)
{
    using Fixed = ts::FixedPoint<int32_t, 3>;

    Fixed n;
    TSUNIT_EQUAL(0, n.toInt());

    n = Fixed(1234, true);
    TSUNIT_EQUAL(1, n.toInt());
    TSUNIT_EQUAL(1234, n.raw());

    n = -12;
    TSUNIT_EQUAL(-12, n.toInt());
    TSUNIT_EQUAL(-12000, n.raw());

    n = 12;
    n += Fixed(4);
    TSUNIT_EQUAL(16, n.toInt());
    TSUNIT_EQUAL(16000, n.raw());

    n += 2;
    TSUNIT_EQUAL(18, n.toInt());
    TSUNIT_EQUAL(18000, n.raw());

    n -= Fixed(6);
    TSUNIT_EQUAL(12, n.toInt());
    TSUNIT_EQUAL(12000, n.raw());

    n -= 2;
    TSUNIT_EQUAL(10, n.toInt());
    TSUNIT_EQUAL(10000, n.raw());

    n *= Fixed(2);
    TSUNIT_EQUAL(20, n.toInt());
    TSUNIT_EQUAL(20000, n.raw());

    n *= 3;
    TSUNIT_EQUAL(60, n.toInt());
    TSUNIT_EQUAL(60000, n.raw());

    n /= Fixed(2);
    TSUNIT_EQUAL(30, n.toInt());
    TSUNIT_EQUAL(30000, n.raw());

    n /= 4;
    TSUNIT_EQUAL(7, n.toInt());
    TSUNIT_EQUAL(7500, n.raw());
}

TSUNIT_DEFINE_TEST(Comparison)
{
    using Fixed = ts::FixedPoint<int32_t, 3>;

    TSUNIT_ASSERT(Fixed(-211) == Fixed(-211));
    TSUNIT_ASSERT(Fixed(1) == Fixed(1000, true));
    TSUNIT_ASSERT(Fixed(21) == 21);
    TSUNIT_ASSERT(21 == Fixed(21));

    TSUNIT_ASSERT(Fixed(-211) != Fixed(-212));
    TSUNIT_ASSERT(Fixed(1) != Fixed(1, true));
    TSUNIT_ASSERT(Fixed(21) != 22);
    TSUNIT_ASSERT(20 != Fixed(21));

    TSUNIT_ASSERT(Fixed(-2) < Fixed(2));
    TSUNIT_ASSERT(Fixed(2) < 3);
    TSUNIT_ASSERT(3 < Fixed(4));

    TSUNIT_ASSERT(Fixed(2) > Fixed(1));
    TSUNIT_ASSERT(Fixed(2) > -2);
    TSUNIT_ASSERT(4 > Fixed(2));

    TSUNIT_ASSERT(Fixed(-2) <= Fixed(2));
    TSUNIT_ASSERT(Fixed(2) <= 3);
    TSUNIT_ASSERT(3 <= Fixed(4));

    TSUNIT_ASSERT(Fixed(-2) <= Fixed(-2));
    TSUNIT_ASSERT(Fixed(2) <= 2);
    TSUNIT_ASSERT(3 <= Fixed(3));

    TSUNIT_ASSERT(Fixed(2) >= Fixed(1));
    TSUNIT_ASSERT(Fixed(2) >= -2);
    TSUNIT_ASSERT(4 >= Fixed(2));

    TSUNIT_ASSERT(Fixed(2) >= Fixed(2));
    TSUNIT_ASSERT(Fixed(2) >= 2);
    TSUNIT_ASSERT(4 >= Fixed(4));
}

TSUNIT_DEFINE_TEST(Bounds)
{
    using Fixed = ts::FixedPoint<int16_t, 3>;

    TSUNIT_EQUAL(-32, Fixed::MIN.toInt());
    TSUNIT_EQUAL(-32768, Fixed::MIN.raw());
    TSUNIT_EQUAL(32, Fixed::MAX.toInt());
    TSUNIT_EQUAL(32767, Fixed::MAX.raw());
}

TSUNIT_DEFINE_TEST(Overflow)
{
    using Fixed1 = ts::FixedPoint<int16_t, 1>;
    using Fixed2 = ts::FixedPoint<int16_t, 2>;

    TSUNIT_ASSERT(!Fixed2(100).mulOverflow(3));
    TSUNIT_ASSERT(Fixed2(100).mulOverflow(4));
    TSUNIT_ASSERT(!Fixed2(100).mulOverflow(-3));
    TSUNIT_ASSERT(Fixed2(100).mulOverflow(-4));

    TSUNIT_ASSERT(!Fixed2(3).divOverflow(Fixed2(1)));
    TSUNIT_ASSERT(Fixed2(4).divOverflow(Fixed2(1)));

    TSUNIT_ASSERT(!Fixed1(10).mulOverflow(Fixed1(30)));
    TSUNIT_ASSERT(Fixed1(10).mulOverflow(Fixed1(40)));
    TSUNIT_ASSERT(!Fixed1(10).mulOverflow(Fixed1(-30)));
    TSUNIT_ASSERT(Fixed1(10).mulOverflow(Fixed1(-40)));

    using Fixed64 = ts::FixedPoint<int64_t, 1>;

    Fixed64 a(29202127);
    int64_t b = 31590000000;
    TSUNIT_ASSERT(a.mulOverflow(b));
}

TSUNIT_DEFINE_TEST(ToString)
{
    using Fix0 = ts::FixedPoint<int32_t, 0>;
    using Fix3 = ts::FixedPoint<int32_t, 3>;

    Fix0 f0;
    Fix3 f3;

    TSUNIT_EQUAL(u"1,234", Fix0(1234).toString());
    TSUNIT_EQUAL(u"   -56,789", Fix0(-56789).toString(10));

    TSUNIT_EQUAL(u"1,234",          Fix3(1234).toString());
    TSUNIT_EQUAL(u"1,234.5",        Fix3(1234500, true).toString());
    TSUNIT_EQUAL(u"1,234.567",      Fix3(1234567, true).toString());
    TSUNIT_EQUAL(u"-1,234.567",     Fix3(-1234567, true).toString());
    TSUNIT_EQUAL(u"-1,234.432",     Fix3(-1234432, true).toString());
    TSUNIT_EQUAL(u"123,456",        Fix3(123456).toString());
    TSUNIT_EQUAL(u"   -56|789.000", Fix3(-56789).toString(14, true, u'|', true, ts::NPOS, true));
    TSUNIT_EQUAL(u"   +56|789.000", Fix3(56789).toString(14, true, u'|', true, ts::NPOS, true));

    TSUNIT_EQUAL(u"1234",     ts::UString::Format(u"%d",   Fix3(1234500, true)));
    TSUNIT_EQUAL(u"1234.5",   ts::UString::Format(u"%s",   Fix3(1234500, true)));
    TSUNIT_EQUAL(u"1,234",    ts::UString::Format(u"%'d",  Fix3(1234500, true)));
    TSUNIT_EQUAL(u"1,234.5",  ts::UString::Format(u"%'s",  Fix3(1234500, true)));
    TSUNIT_EQUAL(u"04D2",     ts::UString::Format(u"%04X", Fix3(1234500, true)));
    TSUNIT_EQUAL(u"1234",     ts::UString::Format(u"%f",   Fix3(1234)));
    TSUNIT_EQUAL(u"1234.5",   ts::UString::Format(u"%f",   Fix3(1234500, true)));
    TSUNIT_EQUAL(u"1234.000", ts::UString::Format(u"%.f",  Fix3(1234)));
    TSUNIT_EQUAL(u"1234.500", ts::UString::Format(u"%.f",  Fix3(1234500, true)));
    TSUNIT_EQUAL(u"1234.50",  ts::UString::Format(u"%.2f", Fix3(1234500, true)));
    TSUNIT_EQUAL(u"1234.500", ts::UString::Format(u"%.3f", Fix3(1234500, true)));
    TSUNIT_EQUAL(u"1234.52",  ts::UString::Format(u"%f",   Fix3(1234520, true)));
    TSUNIT_EQUAL(u"1234.546", ts::UString::Format(u"%f",   Fix3(1234546, true)));
}


TSUNIT_DEFINE_TEST(FromString)
{
    using Fix0 = ts::FixedPoint<int32_t, 0>;
    using Fix3 = ts::FixedPoint<int32_t, 3>;

    Fix0 f0;
    Fix3 f3;

    TSUNIT_ASSERT(f0.fromString(u" 12"));
    TSUNIT_EQUAL(12, f0.toInt());

    TSUNIT_ASSERT(!f0.fromString(u" -12,345 =="));
    TSUNIT_EQUAL(-12345, f0.toInt());

    TSUNIT_ASSERT(f3.fromString(u" 12.3"));
    TSUNIT_EQUAL(12, f3.toInt());
    TSUNIT_EQUAL(12300, f3.raw());

    TSUNIT_ASSERT(!f3.fromString(u" -12,345.6789 =="));
    TSUNIT_EQUAL(-12345, f3.toInt());
    TSUNIT_EQUAL(-12345678, f3.raw());
}
