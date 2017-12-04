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

#include "simpletest.h"
#include <cmath>
#include <limits>
#include <algorithm>

#ifdef NO_SIMPLETESTS
SimpleTestCase::SimpleTestCase (const char*, const char*, int)
#else
SimpleTestCase::SimpleTestCase (const char* name, const char* file, int line)
    : mName{name}, mLine{line}, mFile{file}
#endif
{
}

bool SimpleTestCase::equal(double a, double b)
{
    if (a == b) {
        return true;
    }
    double diff = std::fabs(a - b);
    double max = std::max(fabs(a), fabs(b));
    double rdiff = diff / max;
    return rdiff <= std::numeric_limits<double>::epsilon();
}

#ifdef SIMPLETEST_TESTS
static SimpleTestCase simpleTestCase = SimpleTestCase("t1", __FILE__, __LINE__) = []{
    return true;
};

SIMPLETEST("Test for SimpleTest"){
    return true;
};
#endif
