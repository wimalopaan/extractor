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

#include "commandlineoption.h"

#include <algorithm>

#include "tracer.h"
#include "algorithm.h"

CommandLineOption::CommandLineOption(std::initializer_list<std::string> names, bool hasValue) :
    mNames(names),
    mHasValue(hasValue),
    mIsSet(false)
{
}

bool CommandLineOption::set(bool value)
{
    if (mHasValue) {
        return false;
    }
    mIsSet = value;
    return true;
}

bool CommandLineOption::setValue(const std::string& value)
{
    if (!mHasValue) {
        return false;
    }
    mValue = value;
    return true;
}

bool CommandLineOption::hasValue() const
{
    return mHasValue;
}

const std::string& CommandLineOption::value() const
{
    return mValue;
}

const std::vector<std::string>&CommandLineOption::names() const
{
    return mNames;
}

CommandLineOption::operator bool() const
{
    TRACE;
    trace(Tracer::TraceLevel::trace) << mHasValue << mIsSet << mValue;
    return (mHasValue && !mValue.empty()) || (!mHasValue && mIsSet);
}


bool operator==(const CommandLineOption& option, const std::string& string)
{
    return wm::any_of(option.mNames, [&](const std::string& name){
        return name == string;
    });
}


bool operator==(const std::string& string, const CommandLineOption& option)
{
    return option == string;
}


std::ostream&operator<<(std::ostream& o, const CommandLineOption& option)
{   o << "CommandLineOption" << option.mNames;
    return o;
}


bool operator==(const CommandLineOption& option, char c)
{
    return wm::any_of(option.mNames, [&](const std::string& name){
        return (name.size() == 1) && (name.at(0) == c);
    });
}
