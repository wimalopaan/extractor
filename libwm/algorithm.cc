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

#include <iostream>

#include "algorithm.h"


namespace std {

template<>
istream& operator>>(istream& in, set<std::string>& s) {
    std::string token;

    in >> token;
    if (token != "[") {
        return in;
    }
    in >> token;
    while(token != "]") {
        if (token != ",") {
            s.insert(token);
        }
        in >> token;
    }
    return in;
}

template<>
istream& operator>>(istream& in, vector<std::string>& v) {
    std::string token;

    in >> token;
    if (token != "[") {
        return in;
    }
    in >> token;
    while(token != "]") {
        if (token != ",") {
            v.push_back(token);
        }
        in >> token;
    }
    return in;
}

}

