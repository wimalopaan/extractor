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

#include "asciidocasmgenerator.h"
#include "tracer.h"

#include <fstream>
#include <boost/algorithm/string/replace.hpp>

AsciidocAsmGenerator::AsciidocAsmGenerator(const filesystem::path& file) : 
    mExtractor(file),
    mOutputFilePath((file.parent_path() / file.stem()) += ".axtractor")
  
{
}

bool AsciidocAsmGenerator::generate()
{
    if (!mExtractor.parse()) {
        return false;
    }
    
    auto subDirPath = mExtractor.path().parent_path();
    if (!mSubDirectoryName.empty()) {
        subDirPath /= mSubDirectoryName;
        if (!filesystem::exists(subDirPath)) {
            auto ec = std::error_code{};
            if (!filesystem::create_directory(subDirPath, ec)) {
                Trace(Tracer::TraceLevel::error) << ec.message() << subDirPath;
                return false;
            }
        }
    }
    
    std::ofstream outputFile(mOutputFilePath.string());
    if (outputFile.is_open()) {
        for(const auto& f: mExtractor.functions()) {
            outputFile << f << std::endl;
        }
    }
    else {
        Trace(Tracer::TraceLevel::error) << "Can't open output file: " << mOutputFilePath.string();
    }
    
    
    for(const auto& function: mExtractor.functions()) {
        Trace(Tracer::TraceLevel::debug) << function;
        
        auto snippetFilePath = subDirPath;
        snippetFilePath /= mExtractor.path().filename();
        snippetFilePath += '.';
        snippetFilePath += function.mAssignedName;
        Trace(Tracer::TraceLevel::debug) << snippetFilePath;
        
        std::ofstream fileStream(snippetFilePath.string());
        if (fileStream.is_open()) {
            if (!mSkipSourceCaption) {
                std::string caption = boost::replace_all_copy(mCaption, "%f", mExtractor.path().filename().string());
                std::string s = "[Snippet: " + function.mName + "]";
                boost::replace_all(caption, "%s", s);
                fileStream << caption << "\n";
            }
            fileStream << "[" << mBlockType << "," << mLanguage;
            if (mLineNumbers) {
                fileStream << ",linenums";
            }
            fileStream << ",indent=" << mIndentLevel << "]\n";
            fileStream << mIntro;
            for(size_t l = function.mLineSpan.mBegin; l < (function.mLineSpan.mEnd - 1); ++l) {
                if (std::find(begin(function.mOmittedLines), end(function.mOmittedLines), l+1) == end(function.mOmittedLines)) {
                    fileStream << mExtractor.lines()[l] << '\n';
                }
            }
            fileStream << mExtro;
        }    
        else {
            return false;
        }
    }
    return true;    
}

AsciidocAsmGenerator&AsciidocAsmGenerator::outputFilePath(const filesystem::path& opath)
{
    mOutputFilePath = opath;
    return *this;
}

AsciidocAsmGenerator&AsciidocAsmGenerator::lineNumbers(bool enable)
{
    mLineNumbers = enable;
    return *this;
}
