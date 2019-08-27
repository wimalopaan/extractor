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

#include "filesystem.h"

#include <algorithm>

namespace wm {
namespace filesystem {

DirectoryIteratorProvider<std::filesystem::directory_iterator> dirEntrys(const std::filesystem::path& directory)
{
    return DirectoryIteratorProvider<std::filesystem::directory_iterator>(directory);
}

DirectoryIteratorProvider<std::filesystem::recursive_directory_iterator> rdirEntrys(const std::filesystem::path& directory)
{
    return DirectoryIteratorProvider<std::filesystem::recursive_directory_iterator>(directory);
}

bool isExecutable(const std::filesystem::path& pathname)
{
    if (!std::filesystem::exists(pathname)) {
        return false;
    }
    if (access(pathname.string().c_str(), X_OK) < 0) {
        return false;
    }
    return true;
}

bool isPrefix(const std::filesystem::path& possiblePrefix, const std::filesystem::path& path)
{
    return std::equal(possiblePrefix.begin(), possiblePrefix.end(), path.begin());
}

} // filesystem
} // wm

