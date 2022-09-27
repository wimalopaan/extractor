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

#include "filemagic.h"

#include <iostream>
#include <regex>

#include <magic.h>

#define NO_TRACE
#include "tracer.h"

namespace wm
{

class FileMagicImpl
{
public:
    FileMagicImpl() {
        TRACE;
        if (!mMagic) {
            mMagic = magic_open(MAGIC_SYMLINK | MAGIC_MIME | MAGIC_ERROR);
        }
        if (mMagic && !mLoaded) {
            if (int err = magic_load(mMagic, nullptr); err) {
                std::cerr << magic_error(mMagic) << std::endl;
            }
            else {
                mLoaded = true;
            }
        }
    }
    std::optional<std::string> mime(const std::string& filename) const
    {
        if (!mMagic) {
            return std::optional<std::string>();
        }
        if (!std::filesystem::exists(filename)) {
            return std::optional<std::string>();
        }
        if (const char* const m = magic_file(mMagic, filename.c_str()); !m) {
            std::cerr << magic_error(mMagic) << std::endl;
            return std::optional<std::string>();
        }
        else {
            return std::string(m);
        }
    }
    ~FileMagicImpl() {
        TRACE;
        if (mMagic) {
            magic_close(mMagic);
        }
    }
private:
    inline static magic_t mMagic{nullptr};
    inline static bool mLoaded{false};
};

//magic_t FileMagicImpl::mMagic = nullptr;
//bool FileMagicImpl::mLoaded = false;

static FileMagicImpl mImpl;

std::optional<std::string> FileMagic::mimeString(const std::string& filename)
{
    return mImpl.mime(filename);
}

std::optional<MimeType> FileMagic::mimeType(const std::string& filename)
{
    return MimeType::parse(mimeString(filename));
}

std::optional<MimeType> FileMagic::mimeType(const std::filesystem::directory_entry& dentry)
{
    return mimeType(dentry.path());
}

std::optional<MimeType> FileMagic::mimeType(const std::filesystem::path& path)
{
    return mimeType(path.string());
}

std::ostream& operator<<(std::ostream& out, const MimeType& mimetype)
{
    out << "MimeType[" << mimetype.mType << "/";
    if (!mimetype.mTree.empty()) {
        out << mimetype.mTree << ".";
    }
    out << mimetype.mSubType;
    if (!mimetype.mSuffix.empty()) {
        out << "+" << mimetype.mSuffix;
    }
    for(const auto& pv: mimetype.mParameters) {
        out << ";" << pv.first << "=" << pv.second;
    }
    out << "]";
    return out;
}


// 1: type
// 3: tree
// 5: subtype
// 8: suffix
// 9: av-list
//static std::string rxMimeString = "(\\w+)/((\\w+)\\.)?(([\\w-]+(\\+\\+)?)(\\+(\\w+))?)\\s*((\\s*;\\s*\\w+=[\\w-]+)*)";
static const char* rxMimeString = "(\\w+)/((\\w+)\\.)?(([\\w-]+(\\+\\+)?)(\\+(\\w+))?)\\s*((\\s*;\\s*\\w+=[\\w-]+)*)";
//static std::string rxPVString = ";\\s*(\\w+)=([\\w-]+)";
static const char*  rxPVString = ";\\s*(\\w+)=([\\w-]+)";

MimeType&MimeType::type(const std::string& type)
{
    mType = type;
    return *this;
}

const std::string&MimeType::type() const
{
    return mType;
}

MimeType&MimeType::tree(const std::string& tree)
{
    mTree = tree;
    return *this;
}

const std::string&MimeType::tree() const
{
    return mTree;
}

MimeType&MimeType::subtype(const std::string& subtype)
{
    mSubType = subtype;
    return *this;
}

const std::string&MimeType::subtype() const
{
    return mSubType;
}

MimeType&MimeType::suffix(const std::string& suffix)
{
    mSuffix = suffix;
    return *this;
}

const std::string&MimeType::suffix() const
{
    return mSuffix;
}

MimeType&MimeType::parameter(const std::map<std::string, std::string>& parameter)
{
    mParameters = parameter;
    return *this;
}

MimeType&MimeType::parameter(const std::string& name, const std::string& value)
{
    mParameters[name] = value;
    return *this;
}

const std::map<std::string, std::string>&MimeType::parameter() const
{
    return mParameters;
}

std::optional<MimeType> MimeType::parse(const std::string& string)
{
    std::regex rx(wm::rxMimeString, std::regex::ECMAScript);

    std::smatch matches;
    std::regex_search(string, matches, rx);
    if (matches.size() < 4) {
        return std::optional<MimeType>();
    }
    wm::MimeType mt;
    mt.mType = matches[1];

    if (matches.size() >= 6) {
        mt.mTree = matches[3];
        mt.mSubType = matches[5];
    }
    else {
        mt.mSubType = matches[3];
    }
    if (matches.size() >= 9) {
        mt.mSuffix = matches[8];
    }
    if (matches.size() >= 10) {
        std::string pvlist = matches[9];
        auto startIt = pvlist.cbegin();
        auto endIt = pvlist.cend();
        std::smatch pvmatch;
        std::regex pvRegex(wm::rxPVString, std::regex::ECMAScript);
        while(std::regex_search(startIt, endIt, pvmatch, pvRegex)) {
            mt.mParameters[pvmatch[1]] = pvmatch[2];
            startIt = pvmatch[0].second;
        }
    }
    return mt;
}

std::optional<MimeType> MimeType::parse(const std::optional<std::string>& string)
{
    if (string) {
        return parse(*string);
    }
    return std::optional<MimeType>();
}

std::function<bool(const MimeType&)> MimeType::comparator(int)
{
    return [](const MimeType&) {return true;};
}

bool operator==(const MimeType& lhs, const MimeType& rhs)
{
    return (lhs.mType == rhs.mType) && (lhs.mTree == rhs.mTree) && (lhs.mSubType == rhs.mSubType) &&
            (lhs.mSuffix == rhs.mSuffix) && (lhs.mParameters == rhs.mParameters);
}

bool operator!=(const MimeType& lhs, const MimeType& rhs)
{
    return !(lhs == rhs);
}

}

#ifdef USE_SIMPLETEST
# include "simpletest.h"

SIMPLETEST("FileMagic 01") {
    wm::FileMagic fm;
    auto sfm = fm.mimeString("filemagic.cc");
    if (sfm) {
        auto mt = wm::MimeType::parse(*sfm);
        if (mt) {
            wm::MimeType expected = wm::MimeType().type("text").subtype("x-c++").parameter("charset", "us-ascii");
            if (*mt == expected) {
                return true;
            }
        }
    }
    return false;
};

SIMPLETEST("FileMagic 02") {
    auto mt = wm::MimeType::parse(wm::FileMagic().mimeString("filemagic.h"));
    if (mt && (*mt == wm::MimeType().type("text").subtype("x-c++").parameter("charset", "us-ascii"))) {
        return true;
    }
    return false;
};

SIMPLETEST("FileMagic 03") {
    if (wm::FileMagic().mimeString("bla")) {
        return false;
    }
    return true;
};

SIMPLETEST("MimeType 01") {
    wm::MimeType mt1 = wm::MimeType().tree("tree").type("type").subtype("subtype").suffix("suffix").
                       parameter({{"pn","pv"}, {"a", "b"}});
    wm::MimeType mt2 = wm::MimeType().type("type").tree("tree").subtype("subtype").suffix("suffix").
                       parameter({{"a", "b"}, {"pn","pv"}});
    if (mt1 == mt2) {
        return true;
    }
    return false;
};

SIMPLETEST("rxMime 01") {
    std::string text("type/tree.subtype+suffix; pn=pv; a=b");
    auto mt = wm::MimeType::parse(text);
    if (mt) {
        wm::MimeType ex = wm::MimeType().type("type").tree("tree").subtype("subtype").suffix("suffix").
                                parameter({{"pn","pv"}, {"a", "b"}});
//        std::cout << *mt << std::endl;
        wm::MimeType mt2 = *mt;
//        std::cout << ex << std::endl;
        if (*mt == ex) {
            return true;
        }
    }
    return false;
};

SIMPLETEST("rxMime 02") {
    std::string text("type/subtype; pn=pv");
    auto mt = wm::MimeType::parse(text);
    if (mt) {
        wm::MimeType expected = wm::MimeType().type("type").subtype("subtype").parameter({{"pn", "pv"}});
        if (*mt == expected) {
            return true;
        }
    }
    return false;
};

SIMPLETEST("rxMime 03") {
    std::string text("type/subtype");
    auto mt = wm::MimeType::parse(text);
    if (mt) {
        wm::MimeType expected = wm::MimeType().type("type").subtype("subtype");
        if (*mt == expected) {
            return true;
        }
    }
    return false;
};
#endif
