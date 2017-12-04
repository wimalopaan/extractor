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

#include "asciidocsnippetfilegenerator.h"

#include <fstream>

#include <boost/algorithm/string/replace.hpp>

#include "algorithm.h"
#include "util.h"
#include "tracer.h"

const char* AsciidocSnippetFileGenerator::BlockType = "source";
const char* AsciidocSnippetFileGenerator::Intro = "----\n";
const char* AsciidocSnippetFileGenerator::Extro = "----\n";

AsciidocSnippetFileGenerator::AsciidocSnippetFileGenerator(const filesystem::path& path) :
    mExtractor(path),
    mOutputFilePath((path.parent_path() / path.stem()) += ".extractor")
{
}

bool AsciidocSnippetFileGenerator::generate()
{
    if (!mExtractor.parse()) {
        return false;
    }

    std::ofstream outputFile(mOutputFilePath.string());
    if (outputFile.is_open()) {
        for(const auto& pair: mExtractor.snippets()) {
            outputFile << std::get<CppExtractor::Snippet>(pair) << std::endl;
        }
    }
    else {
        Trace(Tracer::TraceLevel::error) << "Can't open output file: " << mOutputFilePath.string();
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

    Trace(Tracer::TraceLevel::debug) << "Highlighted Lines: " << mExtractor.highlightedLines();

    for(const auto& snippetPair: mExtractor.snippets()) {
        const auto & snippet = std::get<CppExtractor::Snippet>(snippetPair);
        Trace(Tracer::TraceLevel::debug) << snippet;

        auto snippetFilePath = subDirPath;
        snippetFilePath /= mExtractor.path().filename();
        snippetFilePath += '.';
        snippetFilePath += snippet.mName;
        Trace(Tracer::TraceLevel::debug) << snippetFilePath;

        std::vector<size_t> calloutLineNumbers;

        const auto& callouts = mExtractor.callouts();

        std::ofstream fileStream(snippetFilePath.string());
        if (fileStream.is_open()) {
            auto coNumber = size_t{1};
            if (!mSkipSourceCaption) {
                std::string caption = boost::replace_all_copy(mCaption, "%f", mExtractor.path().filename().string());
                if (snippet.mName != "all") {
                    std::string s = "[Snippet: " + snippet.mName + "]";
                    boost::replace_all(caption, "%s", s);
                }
                else {
                    boost::replace_all(caption, "%s", "");
                }
                if (mExtractor.hasOmittedLines()) {
                    boost::replace_all(caption, "%o", "(einige Zeilen nicht dargestellt)");
                }
                else {
                    boost::replace_all(caption, "%o", "");
                }
                fileStream << caption << "\n";
            }
            fileStream << "[" << BlockType << "," << mLanguage;
            if (mLineNumbers) {
                fileStream << ",linenums";
            }

            size_t codeLineNumber = 1;
            std::vector<size_t> highlightedSnippetLines;

            auto codeLines = std::string{};
            for(const auto& lineSpan: snippet.mLineSpans) {
                if (!wm::isFirstElement(lineSpan, snippet.mLineSpans)) {
                    if (mPrintMultiSnippetDelimiter) {
                        codeLines += mMultiSnippetDeliminterString;
                    }
                }
                bool printExcludeMarkerForLine = true;
                for(auto lineNumber = lineSpan.first; lineNumber < lineSpan.second; ++lineNumber) {
                    if (!mExtractor.isLineExcluded(snippet, lineNumber)) {
                        printExcludeMarkerForLine = true;
                        std::string line = mExtractor.fileContents()[lineNumber];
                        if (std::count(mExtractor.highlightedLines().begin(), mExtractor.highlightedLines().end(), lineNumber) > 0) {
                            highlightedSnippetLines.push_back(codeLineNumber);
                        }
                        auto calloutIt = callouts.find(lineNumber);
                        if (calloutIt != callouts.end()) {
                            const CppExtractor::CallOut& co = std::get<CppExtractor::CallOut>(*calloutIt);
                            if (mSkipCallouts) {
                                line.replace(co.mCommentColumn, line.length(), "");
                            }
                            else {
                                line.replace(co.mColumn, line.length(), "<" + std::to_string(coNumber) + ">");
                            }
                            calloutLineNumbers.push_back(lineNumber);
                            ++coNumber;
                        }
                        codeLines += line;
                        codeLines += '\n';
                    }
                    else {
                        if (printExcludeMarkerForLine) {
                            if (mPrintExcludeMarker) {
                                if (mExtractor.hasExcludeText(snippet, lineNumber)) {
                                    codeLines += "// " + mExtractor.excludeText(snippet, lineNumber) + "\n";
                                }
                                else {
                                    codeLines += mExcludeMarker;
                                }
                            }
                            printExcludeMarkerForLine = false;
                        }
                    }
                    ++codeLineNumber;
                }
            }
            if (highlightedSnippetLines.size() > 0) {
                fileStream << ",highlight=\"";
                for(const auto& l: highlightedSnippetLines) {
                    if (!wm::isFirstElement(l, highlightedSnippetLines)) {
                        fileStream << ",";
                    }
                    fileStream << l;
                }
                fileStream << "\"";
            }
            fileStream << ",indent=" << mIndentLevel << "]\n";
            fileStream << Intro;
            fileStream << mAStyle.formattedCText(codeLines).get();
            fileStream << Extro;
            if (!mSkipCallouts) {
                if (calloutLineNumbers.size() > 0) {
                    coNumber = 1;
                    for(const auto& coLine: calloutLineNumbers) {
                        fileStream << "<" << coNumber << "> " << callouts.at(coLine).mText << std::endl;
                        ++coNumber;
                    }
                }
            }
        }
        else {
            return false;
        }
    }
    return true;
}

const std::string& AsciidocSnippetFileGenerator::astyleOptions() const
{
    return mAStyle.options();
}

AsciidocSnippetFileGenerator& AsciidocSnippetFileGenerator::astyleOptions(const std::string& optionsString)
{
    mAStyle.options(optionsString);
    return *this;
}

const std::string& AsciidocSnippetFileGenerator::multiSnippetDelimiter() const
{
    return mMultiSnippetDeliminterString;
}

AsciidocSnippetFileGenerator& AsciidocSnippetFileGenerator::multiSnippetDelimiter(const std::string& delimiter)
{
    mMultiSnippetDeliminterString = delimiter;
    return *this;
}

const std::string&AsciidocSnippetFileGenerator::subDirectoryName() const
{
    return mSubDirectoryName;
}

AsciidocSnippetFileGenerator& AsciidocSnippetFileGenerator::subDirectoryName(const std::string& name)
{
    mSubDirectoryName = name;
    return *this;
}

const std::string& AsciidocSnippetFileGenerator::language() const
{
    return mLanguage;
}

AsciidocSnippetFileGenerator& AsciidocSnippetFileGenerator::language(const std::string& language)
{
    Trace(Tracer::TraceLevel::trace) << language;
    mLanguage = language;
    mAStyle.setLanguage(mLanguage);
    return *this;
}

int AsciidocSnippetFileGenerator::indent() const
{
    return mIndentLevel;
}

AsciidocSnippetFileGenerator& AsciidocSnippetFileGenerator::indent(int level)
{
    mIndentLevel = level;
    return *this;
}

bool AsciidocSnippetFileGenerator::highlightLines() const
{
    return mExtractor.highlightLines();
}

AsciidocSnippetFileGenerator& AsciidocSnippetFileGenerator::highlightLines(bool highlight)
{
    mExtractor.highlightLines(highlight);
    return *this;
}

bool AsciidocSnippetFileGenerator::skipEmptyLines() const
{
    return mExtractor.skipEmptyLines();
}

AsciidocSnippetFileGenerator& AsciidocSnippetFileGenerator::skipEmptyLines(bool skip)
{
    mExtractor.skipEmptyLines(skip);
    return *this;
}

bool AsciidocSnippetFileGenerator::skipBlockComments() const
{
    TRACE;
    return mExtractor.skipBlockComments();
}

AsciidocSnippetFileGenerator&AsciidocSnippetFileGenerator::skipBlockComments(bool skip)
{
    TRACE;
    mExtractor.skipBlockComments(skip);
    return *this;
}

bool AsciidocSnippetFileGenerator::skipSnippetDefinitions() const
{
    return mExtractor.skipSnippetDefinitions();
}

AsciidocSnippetFileGenerator&AsciidocSnippetFileGenerator::skipSnippetDefinitions(bool skip)
{
    mExtractor.skipSnippetDefinitions(skip);
    return *this;
}

bool AsciidocSnippetFileGenerator::skipCallouts() const
{
    return mSkipCallouts;
}

AsciidocSnippetFileGenerator&AsciidocSnippetFileGenerator::skipCallouts(bool skip)
{
    mSkipCallouts = skip;
    return *this;
}

bool AsciidocSnippetFileGenerator::includeOmitted() const
{
    return mExtractor.includeOmittedLines();
}

AsciidocSnippetFileGenerator& AsciidocSnippetFileGenerator::includeOmitted(bool include)
{
    mExtractor.includeOmittedLines(include);
    return *this;
}

bool AsciidocSnippetFileGenerator::printExcludeMarker() const
{
    return mPrintExcludeMarker;
}

AsciidocSnippetFileGenerator& AsciidocSnippetFileGenerator::printExcludeMarker(bool ok)
{
    mPrintExcludeMarker = ok;
    return *this;
}

const std::string&AsciidocSnippetFileGenerator::excludeMarker() const
{
    return mExcludeMarker;
}

AsciidocSnippetFileGenerator&AsciidocSnippetFileGenerator::excludeMarker(const std::string& excludeMarker)
{
    mExcludeMarker = excludeMarker;
    return *this;
}

const filesystem::path&AsciidocSnippetFileGenerator::outputFilePath() const
{
    return mOutputFilePath;
}

AsciidocSnippetFileGenerator& AsciidocSnippetFileGenerator::outputFilePath(const filesystem::path& path)
{
    mOutputFilePath = path;
    return *this;
}

bool AsciidocSnippetFileGenerator::lineNumbers() const
{
    return mLineNumbers;
}

AsciidocSnippetFileGenerator& AsciidocSnippetFileGenerator::lineNumbers(bool numbering)
{
    mLineNumbers = numbering;
    return *this;
}

bool AsciidocSnippetFileGenerator::printMultiSnippetDelimiter() const
{
    return mPrintMultiSnippetDelimiter;
}

AsciidocSnippetFileGenerator& AsciidocSnippetFileGenerator::printMultiSnippetDelimiter(bool print)
{
    mPrintMultiSnippetDelimiter = print;
    return *this;
}

bool AsciidocSnippetFileGenerator::parseOnly()
{
    return mExtractor.parse();
}

std::string AsciidocSnippetFileGenerator::fileContents() const
{
    std::string contents;
    for(const std::string& line: mExtractor.fileContents()) {
        contents += line + '\n';
    }

    return mAStyle.formattedText(contents);
}
