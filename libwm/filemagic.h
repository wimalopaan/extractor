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
#include <map>
#include <functional>

#include <experimental/optional>

//#include <boost/filesystem.hpp>

#include <experimental/filesystem>
using namespace std::experimental;

namespace wm
{

// POD
class MimeType
{
public:
    enum CompareFlag_t {Type = 0x01, Tree = 0x02, Subtype = 0x04, Suffix = 0x08, Parameter = 0x10, All = 0xff};

    MimeType& type(const std::string& type);
    const std::string& type() const;

    MimeType& tree(const std::string& tree);
    const std::string& tree() const;

    MimeType& subtype(const std::string& subtype);
    const std::string& subtype() const;

    MimeType& suffix(const std::string& suffix);
    const std::string& suffix() const;

    MimeType& parameter(const std::map<std::string,std::string>& parameter);
    MimeType& parameter(const std::string& name, const std::string& value);
    const std::map<std::string,std::string>& parameter() const;

    std::string mType;
    std::string mTree;
    std::string mSubType;
    std::string mSuffix;
    std::map<std::string,std::string> mParameters;

    static std::experimental::optional<MimeType> parse(const std::string& string);
    static std::experimental::optional<MimeType> parse(const std::experimental::optional<std::string>& string);

    static std::function<bool(const MimeType&)> comparator(int flags);
};
bool operator==(const MimeType& lhs, const MimeType& rhs);
bool operator!=(const MimeType& lhs, const MimeType& rhs);

std::ostream& operator<<(std::ostream& out, const MimeType& mimetype);

// monostate pattern
class FileMagic
{
public:
    static std::experimental::optional<std::string> mimeString(const std::string& filename);

    static std::experimental::optional<MimeType> mimeType(const std::string& filename);

    static std::experimental::optional<MimeType> mimeType(const filesystem::directory_entry& dentry);
    static std::experimental::optional<MimeType> mimeType(const filesystem::path& path);
private:
};



}
