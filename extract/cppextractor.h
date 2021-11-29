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

#include <string>
#include <vector>
#include <set>
#include <functional>
#include <map>
#include <regex>
#include <filesystem>

class CppExtractor final
{
public:
    class Snippet final {
    public:
        std::vector<std::pair<size_t, size_t>> mLineSpans;
        std::string mName;
        std::set<std::string> mExcludes;
        std::string mTextIfExcluded;

        bool containsLine(size_t lineNumber) const;

        explicit operator bool() const;

        static bool isSnippet(const std::string& line, bool fullTest = false);
        static bool isSnippet(const std::filesystem::path& path, bool fullTest = false);

        static std::vector<Snippet> snippets(const std::filesystem::path& path);
    };

    class CallOut final {
    public:
        size_t mLineNumber;
        size_t mColumn;
        int mNumber;
        std::string mText;
        size_t mCommentColumn;
    };

    explicit CppExtractor(const std::filesystem::path& path);

    CppExtractor(const CppExtractor&) = delete;
    CppExtractor(CppExtractor&&) = delete;
    CppExtractor& operator=(const CppExtractor&) = delete;
    CppExtractor& operator=(CppExtractor&&) = delete;

    bool parse();

    const std::map<size_t, CallOut>& callouts() const;
    const std::map<std::string, Snippet>& snippets() const;
    const std::vector<std::string>& fileContents() const;

    bool isLineExcluded(const Snippet& snippet, size_t lineNumber) const;

    bool hasExcludeText(const Snippet& snippet, size_t lineNumber) const;
    std::string  excludeText(const Snippet& snippet, size_t lineNumber) const;

    const std::filesystem::path& path() const;
    CppExtractor& path(const std::filesystem::path& path);

    CppExtractor& highlightLines(bool highlight);
    bool highlightLines() const;

    const std::vector<size_t>& highlightedLines() const;

    CppExtractor& skipBlockComments(bool skip);
    bool skipBlockComments() const;

    CppExtractor& skipEmptyLines(bool skip);
    bool skipEmptyLines() const;

    CppExtractor& skipSnippetDefinitions(bool skip);
    bool skipSnippetDefinitions() const;

    CppExtractor& includeOmittedLines(bool include);
    bool includeOmittedLines() const;

    bool hasOmittedLines() const;

    void activateLua();
private:
    std::filesystem::path mPath;
    bool mSkipBlockComments{true};
    bool mSkipEmptyLines{true};
    bool mSkipSnippetDefinitions{true};
    bool mIncludeOmittedLines{false};
    bool mHighlightLines{true};
    bool mHasOmittedLines{false};

    std::vector<std::string> mFileContents;
    std::map<size_t, CallOut> mCallOuts; // lineNumber -> CallOut
    std::map<std::string, Snippet> mSnippets; // snippetName -> Snippet
    std::vector<size_t> mHighlightedLines;
    
    bool mLua{false};
};

std::ostream& operator<<(std::ostream& out, const CppExtractor::CallOut&);

std::ostream& operator<<(std::ostream& out, const CppExtractor::Snippet&);
std::istream& operator>>(std::istream& in, CppExtractor::Snippet&);


