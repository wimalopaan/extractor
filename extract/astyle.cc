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

#include "astyle.h"

#include <iostream>

#include "tracer.h"

extern "C" {
// from astyle library
char* AStyleGetVersion();
char* AStyleMain(const char* pSourceIn, const char* pOptions, void (*fpError)(int, char*), char* (*fpAlloc)(unsigned long));
}

static void ASErrorHandler(int errorNumber, char* errorMessage)
{
    Trace(Tracer::TraceLevel::error) << "astyle error: " << errorNumber << " :" << errorMessage << std::endl;
}

static char* ASMemoryAlloc(unsigned long memoryNeeded)
{
    return new(std::nothrow) char[memoryNeeded];
}

AStyle::AStyle() :
    mOptions("-A1")
{
}

const std::string&AStyle::options() const
{
    return mOptions;
}

AStyle& AStyle::options(const std::string& options)
{
    mOptions = options;
    return *this;
}

AStyle& AStyle::setLanguage(const std::string& language)
{
    if (language == "cpp") {
        mLanguage = "c";
    }
    if (language == "java") {
        mLanguage = "java";
    }
    Trace(Tracer::TraceLevel::trace) << mLanguage;
    return *this;
}

std::string AStyle::formattedText(const std::string& unformattedText) const
{
    std::string options = mOptions;
    if (!mLanguage.empty()) {
        options += " --mode=" + mLanguage;
    }
    Trace(Tracer::TraceLevel::trace) << options;
    std::unique_ptr<const char[]> formatted(AStyleMain(unformattedText.c_str(), options.c_str(), ASErrorHandler, ASMemoryAlloc));
    std::string f = formatted.get();
    return f;
}

std::unique_ptr<const char[]> AStyle::formattedCText(const std::string& unformattedText) const
{
    std::string options = mOptions;
    if (!mLanguage.empty()) {
        options += " --mode=" + mLanguage;
    }
    Trace(Tracer::TraceLevel::trace) << options;
    return std::unique_ptr<const char[]>(AStyleMain(unformattedText.c_str(), options.c_str(), ASErrorHandler, ASMemoryAlloc));
}
