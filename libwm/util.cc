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

#include "util.h"
#include <cmath>
#include <limits>
#include <algorithm>
#include <cctype>

#include "simpletest.h"

void replace(std::string& subject, const std::string& search, const std::string& replace) {
    size_t pos = 0;
    while((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
}

bool valuesAreClose(const double a, const double b)
{
    if (a == b) {
        return true;
    }
    const auto diff = std::fabs(a - b);
    const auto max = std::max(fabs(a), fabs(b));
    const auto rdiff = diff / max;
    return rdiff <= std::numeric_limits<double>::epsilon();
}

#ifndef PPC_NO_UNIT_TESTS
# ifdef __GNUC__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Weffc++"
# endif
# ifdef USE_GTEST
#  include <gtest/gtest.h>
# endif
# ifdef __GNUC__
#  pragma GCC diagnostic pop
# endif

#ifdef USE_GTEST
namespace {

class A: public EqualityComparable<A>
{
public:
    A(int v = 0) : mValue{v}
    {
    }
    int mValue;
};

bool operator==(const A& a1, const A& a2)
{
    return a1.mValue == a2.mValue;
}

}

TEST(EqComp, Test1)
{
    A t1{1}, t2{2};

    ASSERT_TRUE(t1 != t2);
}
#endif

#endif


SIMPLETEST("leftTrim1") {
    std::string s = "abc";

    leftTrim(s);

    if (s != "abc") return false;

    return true;
};

SIMPLETEST("leftTrim2") {
    std::string s = "      abc";

    leftTrim(s);

    if (s != "abc") return false;

    return true;
};

SIMPLETEST("leftTrim3") {
    std::string s = " \t \n abc";

    leftTrim(s);

    if (s != "abc") return false;

    return true;
};

SIMPLETEST("rightTrim1") {
    std::string s = "abc";

    rightTrim(s);

    if (s != "abc") return false;

    return true;
};

SIMPLETEST("rightTrim2") {
    std::string s = "abc      ";

    rightTrim(s);

    if (s != "abc") return false;

    return true;
};

SIMPLETEST("rightTrim3") {
    std::string s = "abc  \n\t\r    ";

    rightTrim(s);

    if (s != "abc") return false;

    return true;
};

SIMPLETEST("trim1") {
    std::string s = "abc";

    trim(s);

    if (s != "abc") return false;

    return true;
};

SIMPLETEST("trim2") {
    std::string s = "  abc   ";

    trim(s);

    if (s != "abc") return false;

    return true;
};

SIMPLETEST("trim3") {
    std::string s = "\n\t\r  abc \t\n\r  ";

    trim(s);

    if (s != "abc") return false;

    return true;
};

void leftTrim(std::string& s)
{
    s.erase(s.begin(), std::find_if_not(s.begin(), s.end(), [](char c) -> bool {return std::isspace(c);}));
}

void rightTrim(std::string& s)
{
    s.erase(std::find_if_not(s.rbegin(), s.rend(), [](char c) -> bool {return std::isspace(c);}).base(), s.end());
}


void trim(std::string& s)
{
    leftTrim(s);
    rightTrim(s);
}
