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

#include <iosfwd>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <algorithm>
#include <sstream>

namespace wm {
template<typename Item, typename Container>
bool isFirstElement(const Item& item, const Container& container);
}

namespace std {

template<typename T>
ostream& operator<<(ostream& out, const vector<T>& v) {
    out << "[ ";
    for(const T& item: v) {
        if (!wm::isFirstElement(item, v)) {
            out << " , ";
        }
        out << item;
    }
    out << " ]";
    return out;
}

template<typename T>
ostream& operator<<(ostream& out, const set<T>& v) {
    out << "[ ";
    for(const T& item: v) {
        if (!wm::isFirstElement(item, v)) {
            out << " , ";
        }
        out << item;
    }
    out << " ]";
    return out;
}

template<typename K, typename V>
ostream& operator<<(ostream& out, const map<K,V>& v) {
    out << "[ ";
    for(const auto& item: v) {
        if (!wm::isFirstElement(item, v)) {
            out << " , ";
        }
        out << item;
    }
    out << " ]";
    return out;
}

template<typename T>
istream& operator>>(istream& in, vector<T>& v) {
    std::string token;

    in >> token;
    if (token != "[") {
        return in;
    }
    while(token != "]") {
        T item;
        in >> item;
        v.push_back(item);
        in >> token;
    }
    return in;
}

template<>
istream& operator>>(istream& in, vector<std::string>& v);

template<typename T>
istream& operator>>(istream& in, set<T>& s) {
    std::string token;

    in >> token;
    if (token != "[") {
        return in;
    }
    while(token != "]") {
        T item;
        in >> item;
        s.insert(item);
        in >> token;
    }
    return in;
}

template<>
istream& operator>>(istream& in, set<std::string>& s);

template<typename T>
ostream& operator<<(ostream& out, const vector<std::shared_ptr<T>>& v) {
    out << " [";
    for(const auto& item: v) {
        if (!wm::isFirstElement(item, v)) {
            out << ", ";
        }
        out << *item;
    }
    out << " ]";
    return out;
}

template<typename First, typename Second>
ostream& operator<<(ostream& out, const pair<First, Second>& p) {
    out << "( " << p.first << " , " << p.second << " )";
    return out;
}

template<typename First, typename Second>
istream& operator>>(istream& in, pair<First, Second>& p) {
    std::string token;
    in >> token;
    if (token != "(") {
        return in;
    }
    First first;
    in >> first;
    in >> token;
    if (token != ",") {
        return in;
    }
    Second second;
    in >> second;
    in >> token;
    if (token != ")") {
        return in;
    }
    p.first = first;
    p.second = second;
    return in;
}

} // std

namespace wm {

template<typename Item, typename Container>
bool isFirstElement(const Item& item, const Container& container)
{
//    return &item == &container[0];
    return &item == &(*container.cbegin());
}

template<typename Container, typename Predicate>
auto any_of(const Container& c, const Predicate& p) {
    return std::any_of(std::begin(c), std::end(c), p);
}

template<typename MapType>
std::vector<typename MapType::key_type> keys(const MapType& map) {
    std::vector<typename MapType::key_type> k;
    for(const auto& value: map) {
        k.push_back(value.first);
    }
    return k;
}

template<typename MapType>
std::vector<typename MapType::mapped_type> values(const MapType& map) {
    std::vector<typename MapType::mapped_type> k;
    for(const auto& value: map) {
        k.push_back(value.second);
    }
    return k;
}

template<typename MapType>
class MapKeyIterator  : public MapType::iterator
{
public:
    MapKeyIterator(const typename MapType::iterator& mapIterator) : MapType::iterator(mapIterator) {}
    typename MapType::iterator::value_type::first_type operator*() const {
        return MapType::iterator::operator*().first;
    }
    typename MapType::iterator::value_type::first_type& operator*() {
        return MapType::iterator::operator*().first;
    }
};

template<typename MapType>
class MapKeyIteratorProvider
{
public:
    MapKeyIteratorProvider(MapType& map) : mMap(map) {}
    MapKeyIterator<MapType> begin() const {
        return mMap.begin();
    }
    MapKeyIterator<MapType> end() const {
        return mMap.end();
    }
    MapKeyIterator<MapType> cbegin() const {
        return mMap.cbegin();
    }
    MapKeyIterator<MapType> cend() const {
        return mMap.cend();
    }
private:
    MapType& mMap;
};

template<typename MapType>
MapKeyIteratorProvider<MapType> mapKeys(MapType& map)
{
    return MapKeyIteratorProvider<MapType>(map);
}

template<typename MapType>
class MapValueIterator  : public MapType::iterator
{
public:
    MapValueIterator(const typename MapType::iterator& mapIterator) : MapType::iterator(mapIterator) {}
    typename MapType::iterator::value_type::second_type operator*() const {
        return MapType::iterator::operator*().second;
    }
    typename MapType::iterator::value_type::second_type& operator*() {
        return MapType::iterator::operator*().second;
    }
};

template<typename MapType>
class MapValueIteratorProvider
{
public:
    MapValueIteratorProvider(MapType& map) : mMap(map) {}
    MapValueIterator<MapType> cbegin() const {
        return mMap.cbegin();
    }
    MapValueIterator<MapType> cend() const {
        return mMap.cend();
    }
    MapValueIterator<MapType> begin() const {
        return mMap.begin();
    }
    MapValueIterator<MapType> end() const {
        return mMap.end();
    }
private:
    MapType& mMap;
};

template<typename MapType>
MapValueIteratorProvider<MapType> mapValues(MapType& map)
{
    return MapValueIteratorProvider<MapType>(map);
}

} // wm

