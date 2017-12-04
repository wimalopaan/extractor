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
#include "commandlineparser.h"
#include "filemagic.h"
#include "tracer.h"

int main(int argc, const char** argv) {
    trace().setTraceLevel(Tracer::TraceLevel::error);
    CommandLineParser clp;
    clp.add({"h", "help"});
    clp.add({"v", "verbose"});
    clp.add(CommandLineOption({"o", "output"}, true));
    clp.add({"n", "linenumbers"});

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

    for(const auto& f: clp.positionals()) {
        AsciidocAsmGenerator asmGenerator(f);
        if (CommandLineOption oo = clp.option("o")) {
            asmGenerator.outputFilePath(oo.value());
        }
        if (CommandLineOption ln = clp.option("n")) {
            asmGenerator.lineNumbers(true);
        }
        asmGenerator.generate();
    }    
    return EXIT_SUCCESS;
}
