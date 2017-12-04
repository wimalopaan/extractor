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
#include <string>
#include <chrono>
#include <iostream>

class Timer final
{
public:
    inline explicit Timer(const std::string& name = "") :
        mName{name},
        mBegin{std::chrono::system_clock::now()}
    {

    }
    inline void start()
    {
        mBegin = std::chrono::system_clock::now();
    }
    inline Timer& stop()
    {
        mEnd= std::chrono::system_clock::now();
        return *this;
    }
    inline long microseconds() const
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(mEnd - mBegin).count();
    }
private:
    std::string mName;
    std::chrono::system_clock::time_point mBegin;
    std::chrono::system_clock::time_point mEnd;
};


class ScopedTimer final
{
public:
    inline explicit ScopedTimer(const std::string& name) :
        mTimer{name}
    {
    }
    inline ~ScopedTimer() {
        std::cout << mTimer.stop().microseconds() << std::endl;
    }
private:
    Timer mTimer;
};

