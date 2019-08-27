/*
 * ++C - C++ introduction
 * Copyright (C) 2013, 2014, 2015, 2016, 2017 Wilhelm Meier <wilhelm.meier@hs-kl.de>
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
#include <filesystem>

#include "asmextractor.h"

class AsciidocAsmGenerator final {
public:
    explicit AsciidocAsmGenerator(const std::filesystem::path& file);
    
    bool generate();
    
    AsciidocAsmGenerator& outputFilePath(const std::filesystem::path& opath);
    AsciidocAsmGenerator& lineNumbers(bool enable);
private:
    AsmExtractor mExtractor;

    std::string mSubDirectoryName{".extractor"};
    std::filesystem::path mOutputFilePath;
    bool mSkipSourceCaption = false;
    bool mLineNumbers = true;
    size_t mIndentLevel = 0;
    
    inline static const std::string mLanguage = "asm";
    inline static const std::string mIntro = "----\n";
    inline static const std::string mExtro = "----\n";
    inline static const std::string mBlockType = "source";
    inline static const std::string mCaption{".Zeilen aus der Datei link:{srcbase}/{srcdir}/%f.html[`%f`,window=\"newwindows\"] %s"};
};
