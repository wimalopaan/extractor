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
#include <functional>

class CommandlineOptionSpecifier
{
public:
    CommandlineOptionSpecifier(char letter);
    CommandlineOptionSpecifier(const std::string& longName);

    CommandlineOptionSpecifier& validator(const std::function<bool(const std::string&)>& validator);
    CommandlineOptionSpecifier& operation(const std::function<bool(const std::string&)>& operation);

    CommandlineOptionSpecifier& hasValue(bool hasValue);
    bool hasValue() const;

    CommandlineOptionSpecifier& allowsMultipleOccurrences(bool multiple);
    bool allowsMultipleOccurrences() const;

private:
    const char mLetter = '\0';
    const std::string mLongName;
    std::function<bool(const std::string&)> mValidator = [](const std::string&) {return true;};
    std::function<bool(const std::string&)> mOperation = [](const std::string&) {return true;};

    bool mHasValue = false;
    bool mAllowsMultipleOccurrences = false;
};

