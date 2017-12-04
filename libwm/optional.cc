/*
 * ++C - C++ introduction
 * Copyright (C) 2013, 2014, 2015, 2016 Wilhelm Meier <wilhelm.meier@hs-kl.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "optional.h"

#ifdef USE_BOOSTTEST
# define BOOST_TEST_DYN_LINK
# ifdef __GNUC__
// Vermeiden der boost::unit_test::ut__detail::nil_t Warnung
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Weffc++"
# endif
# ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4668)
#  pragma warning(disable: 4625)
#  pragma warning(disable: 4626)
#  pragma warning(disable: 4265)
#  pragma warning(disable: 4640)
#  pragma warning(disable: 4571)
# endif
# include <boost/test/unit_test.hpp>
#endif

#ifdef USE_GTEST
# include <gtest/gtest.h>
#endif

#ifdef USE_SIMPLETEST
# include "simpletest.h"
#endif

class Test
{
public:
    Test(int v = 0) : mValue(v)
    {
        TRACE;
    }
    int mValue;
};

bool operator==(const Test& lhs, const Test& rhs)
{
    TRACE;
    return lhs.mValue == rhs.mValue;
}

#ifdef USE_GTEST
TEST(Optional, Initialize1)
{
    Optional<int> x1;
    EXPECT_FALSE(x1);

    Optional<int> x2(1);
    EXPECT_TRUE(x2);
    EXPECT_EQ(1, x2);
    EXPECT_EQ(1, *x2);
}

TEST(Optional, Ctor2)
{
    Optional<Test> x1;
    BOOST_CHECK(!x1);

    trace() << "A";
    Optional<Test> x2(1);
    Test x3(1);
    trace() << "B";
    BOOST_CHECK(x2);
    trace() << "C";
    BOOST_CHECK(x2 == x3);
    // nicht moeglich, weil bei Instanziierung eines Templates
    // keine implizite Typwandlung ausgefuehrt wird.
    //    BOOST_CHECK(x2 == 1);

}

#endif

#ifdef USE_BOOSTTEST

BOOST_AUTO_TEST_SUITE(OptionalTestSuite)

BOOST_AUTO_TEST_CASE(Ctor)
{
    Optional<int> x1;
    BOOST_CHECK(!x1);

    Optional<int> x2{1};
    BOOST_CHECK(x2);
    BOOST_CHECK(1 == x2);
    BOOST_CHECK(1 == *x2);
}

BOOST_AUTO_TEST_CASE(Ctor2)
{
    Optional<Test> x1;
    BOOST_CHECK(!x1);

    trace() << "A";
    Optional<Test> x2(1);
    Test x3(1);
    trace() << "B";
    BOOST_CHECK(x2);
    trace() << "C";
    BOOST_CHECK(x2 == x3);
    // nicht moeglich, weil bei Instanziierung eines Templates
    // keine implizite Typwandlung ausgefuehrt wird.
    //    BOOST_CHECK(x2 == 1);

}

BOOST_AUTO_TEST_SUITE_END()

#ifdef __GNUC__
# pragma GCC diagnostic pop
#endif
#ifdef _MSC_VER
# pragma warning(pop)
#endif

#endif
