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

#include "processenvironment.h"

#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <algorithm>

#include "../libwm/algorithm.h"

bool ProcessEnvironment::isFirstInitialized = false;
std::map<std::string, std::string> ProcessEnvironment::mKeyValues;

ProcessEnvironment::ProcessEnvironment()
{
    if (!isFirstInitialized) {
        update();
    }
}

std::vector<std::string> ProcessEnvironment::keys() const
{
    return wm::keys(mKeyValues);
}

bool ProcessEnvironment::contains(const std::string& key) const
{
    return mKeyValues.find(key) != mKeyValues.end();
}

std::experimental::optional<std::string> ProcessEnvironment::value(const std::string& key) const
{
    if (contains(key)) {
        return mKeyValues.at(key);
    }
    return {};
}

void ProcessEnvironment::update() const
{
    const char* const* ep = environ;

    while(ep && *ep) {
        const char* v = strchr(*ep, '=');
        if (v) {
            mKeyValues[std::string{*ep, v}] = std::string{v + 1};
        }
        ++ep;
    }
}

std::ostream& operator<<(std::ostream& out, const ProcessEnvironment& pe)
{
    return out << pe.mKeyValues;
}

