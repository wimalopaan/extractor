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

#include <filesystem>
#include <string>

#include "cppextractor.h"
#include "astyle.h"

class AsciidocSnippetFileGenerator
{
public:
    AsciidocSnippetFileGenerator(const std::filesystem::path& path);

    bool generate();

    const std::string& astyleOptions() const;
    AsciidocSnippetFileGenerator& astyleOptions(const std::string& optionsString);
    AsciidocSnippetFileGenerator& addAstyleOption(const std::string& optionString);

    const std::string& multiSnippetDelimiter() const;
    AsciidocSnippetFileGenerator& multiSnippetDelimiter(const std::string& delimiter);

    const std::string& subDirectoryName() const;
    AsciidocSnippetFileGenerator& subDirectoryName(const std::string& name);

    const std::string& language() const;
    AsciidocSnippetFileGenerator& language(const std::string& language);

    int indent() const;
    AsciidocSnippetFileGenerator& indent(int level);

    bool highlightLines() const;
    AsciidocSnippetFileGenerator& highlightLines(bool highlight);

    bool skipEmptyLines() const;
    AsciidocSnippetFileGenerator& skipEmptyLines(bool skip = true);

    bool skipBlockComments() const;
    AsciidocSnippetFileGenerator& skipBlockComments(bool skip = true);

    bool skipSnippetDefinitions() const;
    AsciidocSnippetFileGenerator& skipSnippetDefinitions(bool skip = true);

    bool skipCallouts() const;
    AsciidocSnippetFileGenerator& skipCallouts(bool skip = true);

    bool includeOmitted() const;
    AsciidocSnippetFileGenerator& includeOmitted(bool include = true);

    bool printExcludeMarker() const;
    AsciidocSnippetFileGenerator& printExcludeMarker(bool ok = true);

    const std::string& excludeMarker() const;
    AsciidocSnippetFileGenerator& excludeMarker(const std::string& excludeMarker);

    const std::filesystem::path& outputFilePath() const;
    AsciidocSnippetFileGenerator& outputFilePath(const std::filesystem::path& path);

    bool lineNumbers() const;
    AsciidocSnippetFileGenerator& lineNumbers(bool numbering = true);

    bool printMultiSnippetDelimiter() const;
    AsciidocSnippetFileGenerator& printMultiSnippetDelimiter(bool print = true);

    bool parseOnly();
    std::string fileContents() const;
private:
    CppExtractor mExtractor;
    AStyle mAStyle;

    std::string mSubDirectoryName{".extractor"};
    std::string mLanguage{"cpp"};
    int mIndentLevel{0};

    std::string mMultiSnippetDeliminterString{"\n// ... lines omitted ...\n\n"};

    std::filesystem::path mOutputFilePath;

    bool mSkipCallouts{false};
    bool mSkipSourceCaption{false};
    bool mPrintExcludeMarker{true};
    bool mLineNumbers{false};
    bool mPrintMultiSnippetDelimiter{true};

    std::string mExcludeMarker{"// ...\n"};

    std::string mCaption{".Zeilen aus der Datei link:{srcbase}/{srcdir}/%f.html[`%f`,window=\"newwindows\"] %s %o"};

    static const char* BlockType;
    static const char* Intro;
    static const char* Extro;
    static const char* FormattedFileExtension;
};

