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

#include <experimental/filesystem>
using namespace std::experimental;

#include "cppextractor.h"
#include "asciidocsnippetfilegenerator.h"
#include "commandlineparser.h"

#include "filemagic.h"
#include "tracer.h"

#include "docopt.h"

// todo: add option to generate link to  source-file

static const char usage[] =
        R"(extractor
        usage:
        )";

int main(int argc, const char** argv)
{
    trace().setTraceLevel(Tracer::TraceLevel::error);

//    auto amap = docopt::docopt(usage, {argv + 1, argv + argc});

    CommandLineParser clp;
    clp.add({"h", "help"});
    clp.add({"v", "verbose"});
    clp.add({"se", "skipEmptyLines", "skipemptylines"});
    clp.add({"sb", "skipBlockComments", "skipblockcomments"});
    clp.add({"ss", "skipSnippetDefs", "skipsnippetdefs"});
    clp.add({"sc", "skipCallouts", "skipcallouts"});
    clp.add({"sm", "skipMultiSnippetDeliminter", "skipdelimiter"});
    clp.add({"sx", "skipExcludeMarker", "skipexclude"});
    clp.add({"sh", "skipHighlighting", "skiphighlight"});
    clp.add({"ee", "enableEmptyLines", "enableemptylines"});
    clp.add({"eb", "enableBlockComments", "enableblockcomments"});
    clp.add({"es", "enableSnippetDefs", "enablesnippetdefs"});
    clp.add({"ec", "enableCallouts", "enablecallouts"});
    clp.add({"eh", "enableHighlighting", "enablehighlight"});
    clp.add({"io", "includeOmitted", "includeomitted"});
    clp.add(CommandLineOption({"in", "indent", "indentlevel"}, true));
    clp.add(CommandLineOption({"l", "lang", "language"}, true));
    clp.add(CommandLineOption({"a", "astyle", "astyleoptions"}, true));
    clp.add(CommandLineOption({"o", "output"}, true));
    clp.add(CommandLineOption({"d", "subdir"}, true));
    clp.add(CommandLineOption({"x", "filtermode", "filteronly"}));
    clp.add(CommandLineOption({"n", "linenums", "linenumbers"}));

    clp.parse(argc, argv);

    if (!clp.invalidOptions().empty()) {
        for(const CommandLineOption& o: clp.invalidOptions()) {
            std::cerr << "invalid option: " << o.names().front() << std::endl;
        }
    }

    if (CommandLineOption ho = clp.option("h")) {
        std::cout << __cplusplus << std::endl;
        std::cout << clp;
        return EXIT_SUCCESS;
    }
    if (CommandLineOption vo = clp.option("v")) {
        trace().setTraceLevel(Tracer::TraceLevel::trace);
        Trace(Tracer::TraceLevel::trace) << vo;
    }

    std::vector<wm::MimeType> sourceMimeTypes = {wm::MimeType().type("text").subtype("x-c"),
                                                 wm::MimeType().type("text").subtype("x-c++"),
                                                 wm::MimeType().type("text").subtype("x-java")
                                                };
    for(const auto& arg: clp.positionals()) {
        Trace(Tracer::TraceLevel::debug) << arg;

        filesystem::path path(arg);
        std::error_code errorCode;
        if (filesystem::exists(path, errorCode)) {
            if (filesystem::is_regular_file(path)) {
                auto mimeType = wm::FileMagic::mimeType(path);
                Trace(Tracer::TraceLevel::trace) << *mimeType;

                std::string language = "cpp";

                if (mimeType) {
                    if (std::count_if(sourceMimeTypes.begin(), sourceMimeTypes.end(), [&](const wm::MimeType& mt) {
                                      return (mt.mType == mimeType->mType) && (mt.mSubType == mimeType->mSubType);}) > 0) {
                        if (mimeType->type() == "text" && mimeType->subtype() == "java") {
                            language = "java";
                        }
                    }
                    if (mimeType->type() == "text" && mimeType->subtype() == "plain") {
                        if (path.extension() == ".java") {
                            language = "java";
                        }
                    }
                }
                else {
                    if (path.extension() == ".java") {
                        language = "java";
                    }
                }

                Trace(Tracer::TraceLevel::trace) << path.extension();

                AsciidocSnippetFileGenerator generator(path);
                generator.language(language);

                if (CommandLineOption lo = clp.option("l")) {
                    generator.language(lo.value());
                }
                if (CommandLineOption se = clp.option("se")) {
                    generator.skipEmptyLines(true);
                }
                if (CommandLineOption sb = clp.option("sb")) {
                    generator.skipBlockComments(true);
                }
                if (CommandLineOption ss = clp.option("ss")) {
                    generator.skipSnippetDefinitions(true);
                }
                if (CommandLineOption sc = clp.option("sc")) {
                    generator.skipCallouts(true);
                }
                if (CommandLineOption sm = clp.option("sm")) {
                    generator.printMultiSnippetDelimiter(false);
                }
                if (CommandLineOption sx = clp.option("sx")) {
                    generator.printExcludeMarker(false);
                }
                if (CommandLineOption sh = clp.option("sh")) {
                    generator.highlightLines(false);
                }
                if (CommandLineOption ee = clp.option("ee")) {
                    generator.skipEmptyLines(false);
                }
                if (CommandLineOption eb = clp.option("eb")) {
                    generator.skipBlockComments(false);
                }
                if (CommandLineOption es = clp.option("es")) {
                    generator.skipSnippetDefinitions(false);
                }
                if (CommandLineOption ec = clp.option("ec")) {
                    generator.skipCallouts(false);
                }
                if (CommandLineOption eh = clp.option("eh")) {
                    generator.highlightLines(true);
                }
                if (CommandLineOption io = clp.option("io")) {
                    generator.includeOmitted(true);
                }
                if (CommandLineOption in = clp.option("in")) {
                    generator.indent(std::stoi(in.value()));
                }
                if (CommandLineOption ao = clp.option("a")) {
                    generator.astyleOptions("-" + ao.value());
                }
                if (CommandLineOption oo = clp.option("o")) {
                    generator.outputFilePath(oo.value());
                }
                if (CommandLineOption sdo = clp.option("d")) {
                    generator.subDirectoryName(sdo.value());
                }
                if (CommandLineOption no = clp.option("n")) {
                    generator.lineNumbers(true);
                }
                if (clp.option("x")) {
                    Trace(Tracer::TraceLevel::trace) << "nosnippets filter mode";
                    if (generator.parseOnly()) {
                        std::cout << generator.fileContents() << std::endl;
                    }
                    else {
                        std::cerr << "Can't process " << path;
                        return EXIT_FAILURE;
                    }
                }
                else {
                    if (!generator.generate()) {
                        std::cerr << "Can't process " << path;
                        return EXIT_FAILURE;
                    }
                }
            }
        }
        else {
            std::cerr << path << errorCode.message();
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
