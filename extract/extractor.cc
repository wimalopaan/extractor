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
#include <regex>
#include <cassert>
#include <regex>
#include <experimental/filesystem>
using namespace std::experimental;
#include "cppextractor.h"
#include "asciidocsnippetfilegenerator.h"
#include "commandlineparser.h"
#include "clara.hpp"
using namespace clara;
#include "filemagic.h"
#include "tracer.h"
#include "docopt.h"
// todo: add option to generate link to  source-file
int main(int argc, char* argv[])
{
    const std::regex isOption{"^-{1,2}"};
    std::vector<wm::MimeType> sourceMimeTypes = {wm::MimeType().type("text").subtype("x-c"),
                                                 wm::MimeType().type("text").subtype("x-c++"),
                                                 wm::MimeType().type("text").subtype("x-java")};
    trace().setTraceLevel(Tracer::TraceLevel::error);
    const std::string sourceFile{argv[--argc]};
    if(std::regex_match(sourceFile, isOption))
    {
        std::cerr << "No source file specified" << std::endl;
        return EXIT_FAILURE;
    }
    const filesystem::path path{sourceFile};
    std::error_code error_code;
    if(!filesystem::exists(path, error_code))
    {
        std::cerr << "Cannot find the file specified." << std::endl;
        return EXIT_FAILURE;
    }
    if(!filesystem::is_regular_file(path))
    {
        std::cerr << "The specified file is not a regular file." << std::endl;
        return EXIT_FAILURE;
    }
    auto mimeType = wm::FileMagic::mimeType(path);
    Trace(Tracer::TraceLevel::trace) << *mimeType;
    std::string language = "cpp";
    bool parseOnly = false;
    if (mimeType)
    {
        if (std::count_if(sourceMimeTypes.begin(), sourceMimeTypes.end(), [&](const wm::MimeType &mt)
        {
            return (mt.mType == mimeType->mType) && (mt.mSubType == mimeType->mSubType);
        }) > 0)
        {
            if (mimeType->type() == "text" && mimeType->subtype() == "java")
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
    Trace(Tracer::TraceLevel::trace) << path.extension();
    AsciidocSnippetFileGenerator generator(path);
    auto cli = Opt([&](bool skipEmptyLines){generator.skipEmptyLines(true);})["-se"]["--skip-empty-lines"]("Skip empty lines?")
               | Opt([&](bool skipBlockComments){generator.skipBlockComments(true);}, "skip block comments")["-sb"]["--skip-block-comments"]("Skip block comments ?")
               | Opt([&](bool skipSnippetDefs){generator.skipSnippetDefinitions(true);}, "skip snippet defs")["-ss"]["--skip-snippet-defs"]("Skip snippet Definitions ?")
               | Opt([&](bool skipCallouts){generator.skipCallouts(false);}, "skip callouts")["-sc"]["--skip-callouts"]("Skip callouts ?")
               | Opt([&](bool skipMultiSnippetDelimiter){generator.printMultiSnippetDelimiter(false);}, "skip multi snippet delimiter")["-sm"]["--skip-multi-snippet-delimiter"]("Skip multi snippet deliminter ?")
               | Opt([&](bool skipExcludeMarker){generator.printExcludeMarker(false);}, "skip exclude marker")["-sx"]["--skip-exclude-marker"]("Skip exclude marker ?")
               | Opt([&](bool skipHighlighting){generator.highlightLines(false);}, "skipHighlighting")["-sh"]["--skip-highlighting"]("Skip highlighting ?")
               | Opt([&](bool enableEmptyLines){generator.skipEmptyLines(false);}, "enable empty lines")["-ee"]["--enable-empty-lines"]("Enable empty lines ?")
               | Opt([&](bool enableBlockComments){generator.skipBlockComments(false);}, "enable block comments")["-eb"]["--enable-block-comments"]("enable block comments ?")
               | Opt([&](bool enableSnippetDefs){generator.skipSnippetDefinitions(false);}, "enable snippet defs")["-es"]["--enable-snippet-defs"]("enable snippet defs ?")
               | Opt([&](bool enableCallouts){generator.skipCallouts(true);}, "enable callouts")["-ec"]["--enable-callouts"]("enable callouts ?")
               | Opt([&](bool enableHighlighting){generator.highlightLines(true);}, "enable highlighting")["-eh"]["--enable-highlighting"]("Enable highlighting?")
               | Opt([&](bool includeOmitted){generator.includeOmitted(true);}, "include omitted")["-io"]["--include-omitted"]("Include omitted?")
               | Opt([&](int indent){if(0 < indent){generator.indent(indent);}}, "indent")["-in"]["--indent"]("indent")
               | Opt([&](std::string lang){if(!lang.empty()){generator.language(lang);}}, "lang")["-l"]["--language"]("Language")
               | Opt([&](std::string astyle){if(!astyle.empty()){generator.astyleOptions("-" + astyle);}}, "asytle")["-a"]["--astyle"]("Astyle ?")
               | Opt([&](std::string output){if(!output.empty()){generator.outputFilePath(output);}}, "output")["-o"]["--output"]("Output")
               | Opt([&](std::string subdir){if(!subdir.empty()){generator.subDirectoryName(subdir);}}, "sub directory")["-d"]["--subdir"]("Subdir")
               | Opt(parseOnly)["-x"]["--filteronly"]("Only filter")
               | Opt([&](bool linenumbers){generator.lineNumbers(true);}, "line numbers")["-n"]["--linenumbers"]("Line numbers");
    auto result = cli.parse( Args( argc, argv ) );
    if(!result)
    {
        std::cerr << "Error in command line: " << result.errorMessage() << std::endl;
        return EXIT_FAILURE;
    }
    Trace(Tracer::TraceLevel::trace) << "nosnippets filter mode";
    if(parseOnly)
    {
        Trace(Tracer::TraceLevel::trace) << "nosnippets filter mode";
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
    return EXIT_SUCCESS;
}