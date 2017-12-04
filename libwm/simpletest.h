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

#pragma once

#include "libwm.h"

#include <iostream>
#include <cassert>
#include <cstdlib>

//#define EXPECT_EQ(a, b) {if (a != b) return false;}

#define CAT2(X, Y) X ## Y
#define CAT(X, Y) CAT2(X, Y)

#define SIMPLETEST(name) static SimpleTestCase CAT(SimpleTestCaseOnLine, __LINE__) = \
    SimpleTestCase(name, __FILE__, __LINE__) = []

class SimpleTestCase {
public:
    SimpleTestCase(const char* name, const char* file, int line);

    template <typename T>
    SimpleTestCase& operator=(T lambda)
    {
        bool noOutput = false;
        bool noTests = false;
        const char* env = getenv("SIMPLETEST");
        if (env) {
            std::string envVar{env};
            if (envVar.find("nooutput") != std::string::npos) {
                noOutput = true;
            }
            if (envVar.find("notests") != std::string::npos) {
                noTests = true;
            }
        }
#ifndef NO_SIMPLETESTS
# ifndef NDEBUG
        if (!noOutput && !noTests) {
            std::cout << "Test(" << mName << "," << mFile << "," << mLine << ")";
        }
# endif
        if (!noTests) {
            if (lambda()) {
# ifndef NDEBUG
                if (!noOutput) {
                    std::cout << " ok" << std::endl;
                }
# endif
            }
            else {
# ifndef NDEBUG
                std::cerr << " failed" << std::endl;
# endif
            }
        }
#else
        (void)lambda;
#endif
        return *this;
    }

    static bool equal(double a, double b);
private:
#ifndef NO_SIMPLETESTS
    const char* mName;
    int mLine;
    const char* mFile;
#endif
};

