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

#include <cassert>
#include <iostream>

#define NO_TRACE

#include "tracer.h"

template<typename T> class Optional;

template<typename T>
bool operator==(const Optional<T>& lhs, const Optional<T>& rhs)
{
    TRACE;
    return lhs && rhs && (*lhs == *rhs);
}

template<typename T>
bool operator==(const T& lhs, const Optional<T>& rhs)
{
    TRACE;
    return rhs && (lhs == *rhs);
}

template<typename T>
bool operator==(const Optional<T>& lhs, const T& rhs)
{
    TRACE;
    return lhs && (*lhs == rhs);
}

template<typename T>
bool operator!=(const Optional<T>& lhs, const Optional<T>& rhs)
{
    TRACE;
    return !(lhs == rhs);
}

template<typename T>
std::ostream& operator<<(std::ostream& lhs, const Optional<T>& rhs)
{
    if (rhs) {
        return lhs << *rhs;
    }
    return lhs << false;
}

template<typename T>
class Optional final
{
public:
    Optional() :
        mValid(false),
        mValue()
    {
        TRACE;
    }
    Optional(const T& value) :
        mValid(true),
        mValue(value)
    {
        TRACE;
    }
    Optional(T&& value) :
        mValid(true),
        mValue(std::move(value))
    {
        TRACE;
    }
    explicit operator bool() const // safe-bool
    {
        TRACE;
        return mValid;
    }
    explicit operator bool() // safe-bool
    {
        TRACE;
        return mValid;
    }
    bool operator!() {
        return !mValid;
    }
    T& operator*()
    {
        assert(mValid);
        return mValue;
    }
    const T& operator*() const
    {
        assert(mValid);
        return mValue;
    }
    T* operator->()
    {
        assert(mValid);
        return &mValue;
    }
    const T* operator->() const
    {
        assert(mValid);
        return &mValue;
    }
    operator T&()
    {
        TRACE;
        return mValue;
    }
    operator const T&() const
    {
        TRACE;
        return mValue;
    }
private:
    bool mValid;
    T mValue;
};

