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

#include <iostream>
#include <vector>
#include <string>

#include "asciidocasmgenerator.h"
//#include "commandlineparser.h"
#include "lyra/lyra.hpp"
#include "filemagic.h"
#include "tracer.h"

using namespace lyra;

int main(int argc, const char** argv) {
    trace().setTraceLevel(Tracer::TraceLevel::error);
    
    bool parseOnly{false};
    bool showHelp{false};
    bool showLineNumbers{false};
    std::string outputFilePath;
    std::string sourceFile;
    
    auto cli =  opt(parseOnly)["-x"]["--filter-only"]("parses input file and prints out result without snippet file generation")
                | opt(showLineNumbers)["-n"]["--line-number"]("show line numbers in extracts")
                | opt(outputFilePath, "output file path")["-o"]["--output"]("extractors output file path")
                | arg(sourceFile, "source file")("source file")
                | help(showHelp);
    
    auto result = cli.parse( args( argc, argv ) );
    
    if(!result) {
        std::cerr << "Error in command line: " << result.errorMessage() << std::endl;
        return EXIT_FAILURE;
    }
    
    if(showHelp) {
        std::cout << cli << std::endl;
        return EXIT_SUCCESS;
    }
    
    if(std::empty(sourceFile)){
        std::cout << "No source file defined" << std::endl;
        std::cout << cli << std::endl;
        return EXIT_FAILURE;
    }
    //    if (CommandLineOption vo = clp.option("v")) {
    //        trace().setTraceLevel(Tracer::TraceLevel::trace);
    //        Trace(Tracer::TraceLevel::trace) << vo;
    //    }
    
    AsciidocAsmGenerator asmGenerator(sourceFile);
    if(!std::empty(outputFilePath)){
        asmGenerator.outputFilePath(outputFilePath);
    }
    if (showLineNumbers) {
        asmGenerator.lineNumbers(true);
    }
    asmGenerator.generate();
    return EXIT_SUCCESS;
}
