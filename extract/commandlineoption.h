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

#include <string>
#include <vector>
#include <initializer_list>
#include <iosfwd>

class CommandLineParser;

class CommandLineOption
{
    friend bool operator==(const CommandLineOption& option, const std::string& string);
    friend bool operator==(const CommandLineOption& option, char c);
    friend std::ostream& operator<<(std::ostream& o, const CommandLineOption& option);
    friend class CommandLineParser;
public:
    CommandLineOption(std::initializer_list<std::string> names, bool hasValue = false);

    bool set(bool value = true);
    bool setValue(const std::string& value);

    bool hasValue() const;
    const std::string& value() const;
    explicit operator bool() const;

    const std::vector<std::string>& names() const;

private:
    inline CommandLineOption() : mHasValue(false), mIsSet(false) {}

    std::vector<std::string> mNames;
    bool mHasValue{false};

    bool mIsSet{false};
    std::string mValue;
};

bool operator==(const CommandLineOption& option, const std::string& string);
bool operator==(const std::string& string, const CommandLineOption& option);

bool operator==(const CommandLineOption& option, char c);

std::ostream& operator<<(std::ostream& o, const CommandLineOption& option);

