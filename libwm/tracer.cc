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

#include "tracer.h"

#include <iostream>

unsigned int Tracer::mNestingLevel = 0;
std::string Tracer::mNestingPrefix = "   ";
const char* Tracer::mEntryMarker = "-->";
const char* Tracer::mExitMarker = "<--";

Tracer::TraceLevel Tracer::globalTraceLevel = Tracer::TraceLevel::trace;
std::ostream* Tracer::traceStream = &std::clog;

Tracer::Tracer(std::string name, TraceLevel level) :
    mFixed{false},
    mName{std::move(name)},
    mTimer{},
    mLevel{level},
    mInsertSpaces{false},
    mInsertEndl{false}
{
    print(mEntryMarker);
    if (!mFixed) {
        ++mNestingLevel;
    }
    mTimer.start();
}

Tracer::Tracer(const char* name, TraceLevel level) :
    Tracer{std::string{name}, level}
{
}

Tracer::~Tracer()
{
    mTimer.stop();
    if (mNestingLevel > 0) {
        if (!mFixed) {
            --mNestingLevel;
            print(mExitMarker, " [us: " + std::to_string(mTimer.microseconds()) + "]") ;
        }
    }
    if (mInsertEndl && traceStream) {
        if (mLevel >= globalTraceLevel) {
            *traceStream << std::endl;
        }
    }
}

Tracer& Tracer::setNestingPrefix(const std::string& prefix)
{
    mNestingPrefix = prefix;
    return *this;
}

Tracer& Tracer::setTraceLevel(Tracer::TraceLevel level)
{
    globalTraceLevel = level;
    return *this;
}

Tracer& Tracer::setStream(std::ostream& stream)
{
    traceStream = &stream;
    return *this;
}

const Tracer::TraceLevel&Tracer::traceLevel() const
{
    return globalTraceLevel;
}

Tracer::Tracer(bool fixed, TraceLevel level, bool insertSpaces, bool insertEndl) :
    mFixed{fixed},
    mName{},
    mTimer{},
    mLevel{level},
    mInsertSpaces{insertSpaces},
    mInsertEndl{insertEndl}
{
    mTimer.start();
}

void Tracer::print(const char* marker, const std::string& trailer) const
{
    if (mLevel < globalTraceLevel) {
        return;
    }
    if (!traceStream) {
        return;
    }
    *traceStream << marker;
    for(unsigned int i = 0; i < mNestingLevel; ++i) {
        *traceStream << mNestingPrefix;
    }
    *traceStream << mName << trailer << std::endl;
}

Tracer trace(Tracer::TraceLevel level, bool insertEndl)
{
    return Tracer{true, level, true, insertEndl};
}

Tracer notrace()
{
    return Tracer{true, Tracer::TraceLevel::error, false, false};
}
