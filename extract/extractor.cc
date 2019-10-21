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
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <regex>
#include <cassert>
#include <regex>
#include <filesystem>
#include <sstream>

#include "cppextractor.h"
#include "asciidocsnippetfilegenerator.h"
#include "commandlineparser.h"
#include "lyra/lyra.hpp"
#include "filemagic.h"
#include "tracer.h"

using namespace lyra;

// todo: add option to generate link to source-file
const std::regex isOption{"^(-{1,2})(.*)"};
std::vector<wm::MimeType> sourceMimeTypes = {wm::MimeType().type("text").subtype("x-c"),
                                             wm::MimeType().type("text").subtype("x-c++"),
                                             wm::MimeType().type("text").subtype("x-java")};
bool checkIfFileWasSpecified(const std::string&);
void checkMimeTypeForLanguage(const std::filesystem::path&, std::string&);

int main(int argc, char* argv[])
{
    trace().setTraceLevel(Tracer::TraceLevel::error);
    //Options
    bool parseOnly{false};
    bool enableSnippetDefinitions{false};
    bool skipCallouts{false};
    bool printMultiSnippetDelimiter{false};
    bool printExcludeMarker{false};
    bool skipLineHighlighting{false};
    bool enableEmptyLines{false};
    bool enableBlockComments{false};
    bool includeOmitted{false};
    bool showLineNumbers{false};
    bool showHelp{false};
    std::string language{"cpp"};
    std::vector<std::string> astyleOptions;
    std::string outputFilePath;
    std::string extractorSubDir;
    std::string sourceFile;
    std::error_code error_code;
    int indent{0};

    auto cli =  opt(parseOnly)["-x"]["--filter-only"]("parses input file and prints out result without snippet file generation")
                | opt(enableEmptyLines)["--enable-empty-lines"]("enable empty lines in snippet")
                | opt(enableSnippetDefinitions)["--enable-snippet-defs"]("enable snippet definitions in snippets")
                | opt(enableBlockComments)["--enable-block-comments"]("enable block comments in snippets")
                | opt(skipCallouts)["--skip-callouts"]("skip callouts")
                | opt(printMultiSnippetDelimiter)["--print-multi-snippet-delim"]("prints multi snippet delimiters")
                | opt(printExcludeMarker)["--print-exclude-marker"]("prints exclude marker")
                | opt(skipLineHighlighting)["--skip-highlighting"]("skip source code line highlighting")
                | opt(includeOmitted)["--include-omitted"]("include omitted lines")
                | opt(language, "language")["-l"]["--language"]("source files language")
                | opt(astyleOptions, "astyle options")["-a"]["--astyle"]("options passed to astyle").cardinality(0)
                | opt(outputFilePath, "output file path")["-o"]["--output"]("extractors output file path")
                | opt(extractorSubDir, "extractor subdirectory")["-d"]["--subdir"]("output directory of generated snippets")
                | opt(showLineNumbers)["-n"]["--line-number"]("show line numbers in extracts")
                | opt(indent, "indentation")["--indent"]("amount of indentation")
                | arg(sourceFile, "source file")("source file")
                | help(showHelp);

    auto result = cli.parse( args( argc, argv ) );

    if(!result)
    {
        std::cerr << "Error in command line: " << result.errorMessage() << std::endl;
        return EXIT_FAILURE;
    }

    if(showHelp){
        std::cout << cli << std::endl;
        return EXIT_SUCCESS;
    }
    
    if(std::empty(sourceFile)){
        std::cout << "No source file defined" << std::endl;
        std::cout << cli << std::endl;
        return EXIT_FAILURE;
    }

    if(!checkIfFileWasSpecified(sourceFile))
    {
        return EXIT_FAILURE;
    }

    std::filesystem::path path{sourceFile};

    if(!std::filesystem::exists(path, error_code)){
        std::cerr << "Cannot find the specified file." << std::endl;
        return EXIT_FAILURE;
    }

    if(!std::filesystem::is_regular_file(path)){
        std::cerr << "The specified file path does not lead to a regular file." << std::endl;
    }

    checkMimeTypeForLanguage(path, language);

    AsciidocSnippetFileGenerator generator{path};
    
    generator.skipEmptyLines(!enableEmptyLines)
        .skipSnippetDefinitions(!enableSnippetDefinitions)
        .skipBlockComments(!enableBlockComments)
        .skipCallouts(skipCallouts)
        .printMultiSnippetDelimiter(printMultiSnippetDelimiter)
        .printExcludeMarker(printExcludeMarker)
        .highlightLines(!skipLineHighlighting)
        .includeOmitted(includeOmitted)
        .lineNumbers(showLineNumbers);

    if(!std::empty(language)){
        generator.language(language);
    }

    if(!std::empty(astyleOptions)){
        std::stringstream astyleSS;

        for(const auto& astyleOption : astyleOptions){
            astyleSS << astyleOption << ',';
        }

        generator.astyleOptions(astyleSS.str());
    }

    if(!std::empty(outputFilePath)){
        generator.outputFilePath(outputFilePath);
    }

    if(!std::empty(extractorSubDir)){
        generator.subDirectoryName(extractorSubDir);
    }

    if(indent > 0){
        generator.indent(indent);
    }

    if(parseOnly)
    {
        Trace(Tracer::TraceLevel::trace) << "no snippets filter mode";
        if (generator.parseOnly())
        {
            std::cout << generator.fileContents() << std::endl;
        }
        else
        {
            std::cerr << "Can't process " << path;
            return EXIT_FAILURE;
        }
    }
    else
    {
        if(!generator.generate())
        {
            std::cerr << "Can't process " << path;
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

bool checkIfFileWasSpecified(const std::string& sourceFile)
{
    if(std::regex_match(sourceFile, isOption))
    {
        std::cerr << "No source file specified" << std::endl;
        return false;
    }
    return true;
}

void checkMimeTypeForLanguage(const std::filesystem::path& path, std::string& language)
{
    auto mimeType = wm::FileMagic::mimeType(path);
    Trace(Tracer::TraceLevel::trace) << *mimeType;
    if (mimeType)
    {
        if (std::count_if(sourceMimeTypes.begin(), sourceMimeTypes.end(), [&](const wm::MimeType &mt)
        {
            return (mt.mType == mimeType->mType) && (mt.mSubType == mimeType->mSubType);
        }) > 0)
        {
            if(mimeType->type() == "text" && mimeType->subtype() == "java")
            {
                language = "java";
            }
        }
        if (mimeType->type() == "text" && mimeType->subtype() == "plain")
        {
            if (path.extension() == ".java")
            {
                language = "java";
            }
        }
    }
    else
    {
        if (path.extension() == ".java")
        {
            language = "java";
        }
    }
}
