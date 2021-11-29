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

#include "cppextractor.h"

#include <fstream>
#include <regex>

#include <boost/algorithm/string.hpp>

#include "algorithm.h"
#include "tracer.h"

enum class ParsingStatus {Normal, BlockComment};
enum class SectionHighlight {Included, Excluded};

CppExtractor::CppExtractor(const std::filesystem::path& path) :
    mPath(path),
    mSkipBlockComments(true),
    mSkipEmptyLines(true),
    mSkipSnippetDefinitions(true)
{
}

const std::map<size_t, CppExtractor::CallOut>& CppExtractor::callouts() const
{
    return mCallOuts;
}

const std::map<std::string, CppExtractor::Snippet>& CppExtractor::snippets() const
{
    return mSnippets;
}

const std::vector<std::string>& CppExtractor::fileContents() const
{
    return mFileContents;
}

bool CppExtractor::isLineExcluded(const CppExtractor::Snippet& snippet, size_t lineNumber) const
{
    for(const auto& excludeName: snippet.mExcludes) {
//        if (auto excludeIt = mSnippets.find(excludeName); excludeIt != mSnippets.end()) {
        auto excludeIt = mSnippets.find(excludeName);
        if (excludeIt != mSnippets.end()) {
            if (std::get<Snippet>(*excludeIt).containsLine(lineNumber)) {
                return true;
            }
        }
        else {
            Trace(Tracer::TraceLevel::warning) << "no exclude" << excludeName;
        }
    }
    return false;
}

bool CppExtractor::hasExcludeText(const Snippet& snippet, size_t lineNumber) const
{
    for(const auto& excludeName: snippet.mExcludes) {
        auto excludeIt = mSnippets.find(excludeName);
        if (excludeIt != mSnippets.end()) {
            if (std::get<Snippet>(*excludeIt).containsLine(lineNumber)) {
                return !std::get<Snippet>(*excludeIt).mTextIfExcluded.empty();
            }
        }
        else {
            Trace(Tracer::TraceLevel::warning) << "no exclude" << excludeName;
        }
    }
    return false;
}

std::string CppExtractor::excludeText(const Snippet& snippet, size_t lineNumber) const
{
    for(const auto& excludeName: snippet.mExcludes) {
        auto excludeIt = mSnippets.find(excludeName);
        if (excludeIt != mSnippets.end()) {
            if (std::get<Snippet>(*excludeIt).containsLine(lineNumber)) {
                return std::get<Snippet>(*excludeIt).mTextIfExcluded;
            }
        }
        else {
            Trace(Tracer::TraceLevel::warning) << "no exclude" << excludeName;
        }
    }
    return {};
}


const std::filesystem::path& CppExtractor::path() const
{
    return mPath;
}

CppExtractor& CppExtractor::path(const std::filesystem::path& path){
    mPath = path;

    return *this;
}

CppExtractor& CppExtractor::highlightLines(bool highlight)
{
    mHighlightLines = highlight;
    return *this;
}

bool CppExtractor::highlightLines() const
{
    return mHighlightLines;
}

const std::vector<size_t>& CppExtractor::highlightedLines() const
{
    return mHighlightedLines;
}

bool CppExtractor::skipBlockComments() const
{
    TRACE;
    return mSkipBlockComments;
}

CppExtractor& CppExtractor::skipBlockComments(bool skip)
{
    TRACE;
    mSkipBlockComments = skip;
    return *this;
}

bool CppExtractor::skipEmptyLines() const
{
    return mSkipEmptyLines;
}

CppExtractor& CppExtractor::skipSnippetDefinitions(bool skip)
{
    mSkipSnippetDefinitions = skip;
    return *this;
}

bool CppExtractor::skipSnippetDefinitions() const
{
    return mSkipSnippetDefinitions;
}

bool CppExtractor::includeOmittedLines() const
{
    return mIncludeOmittedLines;
}

CppExtractor& CppExtractor::includeOmittedLines(bool include)
{
    mIncludeOmittedLines = include;
    return *this;
}

bool CppExtractor::hasOmittedLines() const
{
    return mHasOmittedLines;
}

CppExtractor& CppExtractor::skipEmptyLines(bool skip)
{
    mSkipEmptyLines = skip;
    return *this;
}

void CppExtractor::activateLua() {
    mLua = true;
}

bool CppExtractor::parse()
{
//    std::regex startBlockComment(R"(^\s*/\*([^+]|\s*$))");                              // match: $<ws>/* (bei folgendem + wird der Kommentar drin gelassen)
    std::regex startBlockComment(R"(^\s*/\*[^+]{0,1}\s*$)");                              // match: $<ws>/* (bei folgendem + wird der Kommentar drin gelassen)
    std::regex startBlockCommentLua(R"(^\s*--\[\[[^+]{0,1}\s*$)");                              // match: $<ws>/* (bei folgendem + wird der Kommentar drin gelassen)
    std::regex endBlockComment(R"(^\s*\*/)");                                            // match: $<ws>*/
    std::regex endBlockCommentLua(R"(^\s*--\]\])");                                            // match: $<ws>*/
    std::regex callOutRegex(R"(//\s*(<([0-9]*)>)\s*(.*))");                              // match: //<ws><<CoNumber>><ws><CoText>
    std::regex callOutRegexLua(R"(--\s*(<([0-9]*)>)\s*(.*))");                              // match: //<ws><<CoNumber>><ws><CoText>
    std::regex snippetStartRegex(R"(^\s*//\[)");                                        // match: $<ws>//[
    std::regex snippetStartRegexLua(R"(^\s*--\{)");                                        // match: $<ws>//[
    std::regex snippetEndRegex(R"(^\s*//\])");                                           // match: $<ws>//]
    std::regex snippetEndRegexLua(R"(^\s*--\})");                                           // match: $<ws>//]
    std::regex omitLineRegex(R"(//-\s*$)");                                               // match: //-
    std::regex highlightRegex(R"(//\*)");                                                 // match: //*
    std::regex highlightSectionIncludeRegex(R"(\s\+\*)");                            // match: +*
    std::regex highlightSectionExcludeRegex(R"(\s-\*)");                              // match: -*
//    std::regex highlightInlineMatchRegex(R"(//\*=)");
    std::string scopeSymbol{"::"};
    std::string scopeSymbolReplacement{"_"};        

    if (mLua) {
        startBlockComment = startBlockCommentLua;
        endBlockComment = endBlockCommentLua;
        callOutRegex = callOutRegexLua;
        snippetStartRegex = snippetStartRegexLua;
        snippetEndRegex = snippetEndRegexLua;
    }
    
    Trace(Tracer::TraceLevel::trace) << "skip block comments" << mSkipBlockComments;
    
    std::ifstream fileStream(mPath.string());
    if (fileStream.is_open()) {
        auto status = ParsingStatus::Normal;
        std::string line = std::string{};
        auto lineNumber = size_t{0};
        auto fileLineNumber = size_t{0};

        auto snippets = std::stack<std::pair<size_t, std::string>>{};
        auto excludes = std::map<std::string, std::set<std::string>>{};
        auto excludeTexts = std::map<std::string, std::string>{};
        auto matches = std::smatch{};

        std::stack<SectionHighlight> sectionHighlight;

        while (std::getline(fileStream, line)) {
            ++fileLineNumber;
            if (mSkipEmptyLines && line.empty()) {
                continue;
            }
            Trace(Tracer::TraceLevel::trace) << line;
            switch(status) {
            case ParsingStatus::Normal:
                if (std::regex_search(line, matches, highlightRegex)) {
                    if(mHighlightLines){
                        mHighlightedLines.push_back(lineNumber);
                    }

                    std::string oline(line.cbegin(), matches[0].first);
                    line = std::move(oline);
                }
                if (std::regex_search(line, matches, omitLineRegex)) {
                    if (mIncludeOmittedLines) {
                        std::string oline(line.cbegin(), matches[0].first);
                        line = std::move(oline);
                    }
                    else {
                        mHasOmittedLines = true;
                        continue;
                    }
                }
                if (mSkipBlockComments && std::regex_search(line, startBlockComment)) {
                    Trace(Tracer::TraceLevel::trace) << "block comment start";
                    status = ParsingStatus::BlockComment;
                    continue;
                }
                if (std::regex_search(line, matches, callOutRegex)) {
                    if (matches.size() >= 4) {
                        decltype(std::stoi(matches[2])) number = -1;
                        if (matches[2].length() > 0) {
                            number = std::stoi(matches[2]);
                        }
                        mCallOuts[lineNumber] = CallOut{lineNumber, static_cast<size_t>(matches[1].first - line.begin()),
                                number, matches[3], static_cast<size_t>(matches[0].first - line.begin())};
                }
            }
            else if (std::regex_search(line, matches, snippetStartRegex)) {

                std::string remainingLine(matches[0].second, line.cend());
                boost::trim(remainingLine); // split retains leading whitespace

                if(std::regex_search(remainingLine, matches, highlightSectionIncludeRegex)) {
                    sectionHighlight.push(SectionHighlight::Included);
                }
                else if(std::regex_search(remainingLine, matches, highlightSectionExcludeRegex)) {
                    sectionHighlight.push(SectionHighlight::Excluded);
                }
                else if(!std::empty(sectionHighlight)) {
                    sectionHighlight.push(sectionHighlight.top());
                }

                std::vector<boost::iterator_range<std::string::iterator>> names;
                boost::split(names, remainingLine, boost::is_any_of(" \t"), boost::token_compress_on);
                for(const auto& p: names) {
                    Trace(Tracer::TraceLevel::trace) << "snippet:" << p;
                }
                if ((names.size() > 0) && (names[0].size() > 0)) { // valid snippet name
                    const std::string snippetName(names[0].begin(), names[0].end());

                    if(boost::contains(snippetName, scopeSymbol)){
                        boost::replace_all(const_cast<std::string&>(snippetName), scopeSymbol, scopeSymbolReplacement);
                    } 

                    snippets.push(std::make_pair(lineNumber, snippetName));

                    for(auto it = ++names.begin(); it != names.end(); ++it) {
                        const std::string excludeName(it->begin(), it->end());
                        if (excludeName.size() > 0) {
                            Trace(Tracer::TraceLevel::trace) << "exclude:" << excludeName;
                            if (excludeName[0] == '-') {
                                auto insertionResult = excludes[snippetName].insert(excludeName.substr(1));
                                if (!std::get<bool>(insertionResult)) {
                                    Trace(Tracer::TraceLevel::warning) << "exclude" << excludeName << "already excluded from" << snippetName;
                                }
                            }
                            else if (excludeName == ":") {
                                auto it2 = it;
                                ++it2;
                                std::string exText(it2->begin(), remainingLine.end());

                                Trace(Tracer::TraceLevel::info) << "exclude text found: "<< exText;

                                excludeTexts[snippetName] = exText;

                                break;
                            }
                            else {
                                Trace(Tracer::TraceLevel::warning) << "exclude" << excludeName << "doesn't begin with dash";

                            }
                        }
                    }
                    if (mSkipSnippetDefinitions) {
                        continue;
                    }
                }
                else {
                    Trace(Tracer::TraceLevel::warning) << "Line: " << fileLineNumber << "Start snippet without name";
                    snippets.push({lineNumber, "unnamed"});
                }
            }
            else if (std::regex_search(line, matches, snippetEndRegex)) {
                if (matches.size() >= 1) {

                    if(!std::empty(sectionHighlight)) {
                        sectionHighlight.pop();
                    }

                    if (snippets.size() >= 1) {
                        const auto& snippetPair = snippets.top();
                        auto it = mSnippets.find(std::get<std::string>(snippetPair));
                        if (it != mSnippets.end()) {
                            Snippet& s = it->second;
                            s.mLineSpans.push_back({std::get<size_t>(snippetPair), lineNumber});
                            auto excludeIt = excludes.find(s.mName);
                            if (excludeIt != excludes.end()) {
                                s.mExcludes.insert(excludeIt->second.cbegin(), excludeIt->second.cend());
                            }
                        }
                        else {
                            Snippet s{{std::make_pair(std::get<size_t>(snippetPair), lineNumber)},
                                      std::get<std::string>(snippetPair), {},
                                            excludeTexts[std::get<std::string>(snippetPair)] };

                            auto excludeIt = excludes.find(s.mName);
                            if (excludeIt != excludes.end()) {
                                s.mExcludes.insert(excludeIt->second.cbegin(), excludeIt->second.cend());
                            }

                            mSnippets.emplace(std::get<std::string>(snippetPair), s);
                        }
                        snippets.pop();
                    }
                    else {
                        Trace(Tracer::TraceLevel::warning) << "Line: " << fileLineNumber << "Unmatched snippet";
                    }
                    if (mSkipSnippetDefinitions) {
                        continue;
                    }
                }
            }

            if(mHighlightLines && !std::empty(sectionHighlight) && sectionHighlight.top() == SectionHighlight::Included){
                mHighlightedLines.push_back(lineNumber);
            }
            ++lineNumber;
            mFileContents.push_back(line);
            break;
            case ParsingStatus::BlockComment:
                if (mSkipBlockComments && std::regex_search(line, endBlockComment)) {
                    Trace(Tracer::TraceLevel::trace) << "block comment end";
                    status = ParsingStatus::Normal;
                }
                continue;
                break;
            default:
                assert(false);
                break;
        }
    }
    if (snippets.size() > 0) {
        Trace(Tracer::TraceLevel::error) << "wrong snippet nesting";
        return false;
    }
    mSnippets.emplace("all", Snippet{{{0, mFileContents.size()}}, "all", {}, {} });
}
else {
return false;
}
return true;
}

std::ostream& operator<<(std::ostream& out, const CppExtractor::CallOut& co)
{
    return out << "CallOut[" << co.mLineNumber << "," << co.mColumn << "," << co.mNumber << "," << co.mText << "]";
}


std::ostream& operator<<(std::ostream& out, const CppExtractor::Snippet& s)
{
    out << "Snippet [ " << s.mName << " ";
    out << s.mLineSpans << " ";
    out << "exclude " << s.mExcludes << " ] ";
    return out;
}

std::istream& operator>>(std::istream& in, CppExtractor::Snippet& s)
{
    s.mLineSpans.clear();

    std::string token;
    CppExtractor::Snippet snippet;

    in >> token;
    if (token != "Snippet") {
        return in;
    }

    in >> token;
    if (token != "[") {
        return in;
    }

    in >> token;
    if (token.length() == 0) {
        return in;
    }
    snippet.mName = token;

    std::vector<std::pair<size_t, size_t>> lineSpans;

    in >> lineSpans;
    if (lineSpans.size() == 0) {
        return in;
    }
    snippet.mLineSpans = lineSpans;

    in >> token;
    if (token != "exclude") {
        return in;
    }

    std::set<std::string> excludes;
    in >> excludes;
    snippet.mExcludes = excludes;

    in >> token;
    if (token != "]") {
        return in;
    }

    s = snippet;
    return in;
}


CppExtractor::Snippet::operator bool() const
{
    return mLineSpans.size() > 0;
}

bool CppExtractor::Snippet::containsLine(size_t lineNumber) const
{
    for(const auto& lineSpan: mLineSpans) {
        if ((lineNumber >= lineSpan.first) && (lineNumber < lineSpan.second)) {
            return true;
        }
    }
    return false;
}

bool CppExtractor::Snippet::isSnippet(const std::string& line, bool fullTest)
{
    if (!fullTest) {
        return line.find("Snippet") != std::string::npos;
    }
    std::regex snippetLineRegEx("^\\s*Snippet\\s+\\[\\s+\\w+\\s+\\[\\s+\\(\\s+\\d+\\s+,\\s+\\d+\\s+\\)(\\s+,\\s+\\(\\s+\\d+\\s+,\\s+\\d+\\s+\\))*\\s+\\]\\s+\\]\\s+");
    return std::regex_match(line, snippetLineRegEx);
}

bool CppExtractor::Snippet::isSnippet(const std::filesystem::path& path, bool fullTest)
{
    std::ifstream fileStream(path.string());
    if (fileStream.is_open()) {
        while(!fileStream.eof()) {
            std::string line;
            std::getline(fileStream, line);
            if (!line.empty()) {
                if (!CppExtractor::Snippet::isSnippet(line)) {
                    return false;
                }
                if (!fullTest) {
                    return true;
                }
            }
        }
        return true;
    }
    return false;
}

std::vector<CppExtractor::Snippet> CppExtractor::Snippet::snippets(const std::filesystem::path& path)
{
    std::vector<CppExtractor::Snippet>  snippets;

    if (!CppExtractor::Snippet::isSnippet(path)) {
        return snippets;
    }

    std::ifstream fileStream(path.string());
    if (fileStream.is_open()) {
        while(!fileStream.eof()) {
            CppExtractor::Snippet snippet;
            fileStream >> snippet;
            if (snippet) {
                snippets.push_back(snippet);
            }
        }
    }
    return snippets;
}
