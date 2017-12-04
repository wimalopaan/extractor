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

#include "commandlineparser.h"

#include <cassert>
#include <algorithm>
#include <iostream>

CommandLineParser::CommandLineParser()
{
}

bool CommandLineParser::parse(int argc, const char** argv)
{
    mArguments.clear();
    std::copy(argv + 1, argv + argc, std::back_inserter(mArguments));

    return parse();
}

bool CommandLineParser::parse(const std::vector<std::string>& args)
{
    mArguments = args;
    return parse();
}

CommandLineParser& CommandLineParser::add(const CommandLineOption& option)
{
    mOptions.push_back(option);
    return *this;
}

CommandLineOption CommandLineParser::option(const std::string& name) const
{
    auto it = std::find(mOptions.cbegin(), mOptions.cend(), name);
    if (it != mOptions.cend()) {
        return *it;
    }
    return CommandLineOption();
}

const std::vector<CommandLineOption>&CommandLineParser::options() const
{
    return mOptions;
}

const std::vector<std::string>& CommandLineParser::positionals() const
{
    return mPositionals;
}

const std::vector<CommandLineOption>& CommandLineParser::invalidOptions() const
{
    return mInvalidOptions;
}

bool CommandLineParser::parse()
{
    auto argIt = mArguments.cbegin();
    while(argIt != mArguments.cend()) {
        const std::string& arg = *argIt++;
        if (!arg.empty()) {
            assert(arg.size() >= 1);
            if (arg.at(0) != '-') {
                mPositionals.push_back(arg);
            }
            else {
                assert(arg.at(0) == '-');
                if (arg.size() >= 2) {
                    if (arg.at(1) == '-') { // long option
                        std::string optionName = arg.substr(2, std::string::npos);
                        auto it = std::find(mOptions.begin(), mOptions.end(), optionName);
                        if (it != mOptions.end()) {
                            CommandLineOption& co = *it;
                            if (co.hasValue()) {
                                co.setValue(*argIt++);
                            }
                            else {
                                co.set();
                            }
                        }
                        else { // no such option
                            mInvalidOptions.push_back(CommandLineOption({optionName}));
                        }
                    }
                    else { // short option
                        char optLetter = arg.at(1);
                        auto it = std::find(mOptions.begin(), mOptions.end(), optLetter);
                        if (it != mOptions.end()) {
                            CommandLineOption& co = *it;
                            if (arg.size() >= 3) { // has value
                                std::string optionValue = arg.substr(2, std::string::npos);
                                if (!co.setValue(optionValue)) {
                                    mInvalidOptions.push_back(co);
                                }
                            }
                            else { // no value
                                if (!co.set()) {
                                    mInvalidOptions.push_back(co);
                                }
                            }
                        }
                        else { // no such option
                            mInvalidOptions.push_back(CommandLineOption({std::string(1, optLetter)}));
                        }
                    }
                }
                else {
                    // single dash
                }
            }
        }
    }
    return true;
}


std::ostream&operator<<(std::ostream& o, const CommandLineParser& parser)
{
    for(const auto& opt: parser.options()) {
        o << opt << std::endl;
    }
    return o;
}
