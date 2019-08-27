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

#include <unistd.h>
#include <filesystem>

namespace wm {
namespace filesystem {

template<typename It>
class DirectoryIteratorProvider
{
public:
    explicit DirectoryIteratorProvider(const std::filesystem::path& directory) : mPath(directory) {}
    It begin() const {return It(mPath);}
    It end() const {return It();}
private:
    const std::filesystem::path& mPath;
};

DirectoryIteratorProvider<std::filesystem::directory_iterator> dirEntrys(const std::filesystem::path& directory);
DirectoryIteratorProvider<std::filesystem::recursive_directory_iterator> rdirEntrys(const std::filesystem::path& directory);

bool isExecutable(const std::filesystem::path& pathname);
bool isPrefix(const std::filesystem::path& possiblePrefix, const std::filesystem::path& path);

} // filesystem
} // wm

