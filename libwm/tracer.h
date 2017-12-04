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

#include "libwm.h"
#include "executiontimer.h"
#include <string>

#ifdef QT_VERSION
# include <QString>
# include <QBuffer>
# include <QDataStream>
# include <QDebug>
#endif

#ifdef _MSC_VER
# define __PRETTY_FUNCTION__ __FUNCTION__
#endif

# ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4003)
# endif

#if !(defined(NDEBUG) || defined(NO_TRACE))
# define TRACE Tracer __t(std::string(__PRETTY_FUNCTION__))
# define TRACE1(s) Tracer __t1(s);
# define TRACE2(s) Tracer __t2(std::string(__PRETTY_FUNCTION__) + " : " + s);
# define Trace(level) trace(level) << __PRETTY_FUNCTION__
#else
# define TRACE
# define TRACE1
# define TRACE2
# define Trace(level) notrace()
#endif

#include <iostream>

class Tracer;

class Tracer final
{
public:
    enum class TraceLevel {trace, debug, info, warning, error};

    friend Tracer trace(TraceLevel level, bool);
    friend Tracer notrace();

    explicit Tracer(const char* name, TraceLevel level = TraceLevel::trace);
    explicit Tracer(std::string name, TraceLevel level = TraceLevel::trace);

    ~Tracer();

    Tracer& setNestingPrefix(const std::string& prefix);
    Tracer& setTraceLevel(TraceLevel level);
    Tracer& setStream(std::ostream& stream);

    const TraceLevel& traceLevel() const;

    typedef std::ostream& (*ioFunctor)(std::ostream&);

#if !(defined(NDEBUG) || defined(NO_TRACE))
    template<typename T>
    const Tracer& operator<<(const T& value) const
    {
        if (mLevel < globalTraceLevel) {
            return *this;
        }
        if (traceStream) {
            for(unsigned int i = 0; i < mNestingLevel; ++i) {
                *traceStream << mNestingPrefix;
            }
            *traceStream << value;
            if (mInsertSpaces) {
                *traceStream << " ";
            }
        }
        return *this;
    }
    const Tracer& operator<<(ioFunctor functor) const
    {
        if (mLevel < globalTraceLevel) {
            return *this;
        }
        if (traceStream) {
            for(unsigned int i = 0; i < mNestingLevel; ++i) {
                *traceStream << mNestingPrefix;
            }
            functor(*traceStream);
            if (mInsertSpaces) {
                *traceStream << " ";
            }
        }
        return *this;
    }
# ifdef QT_VERSION
    const Tracer& operator<<(const QString& value) const
    {
        if (mLevel < globalTraceLevel) {
            return *this;
        }
        if (traceStream) {
            for(unsigned int i = 0; i < mNestingLevel; ++i) {
                *traceStream << mNestingPrefix;
            }
            *traceStream << qPrintable(value);
            if (mInsertSpaces) {
                *traceStream << " ";
            }
        }
        return *this;
    }
    const Tracer& operator<<(const QStringList& value) const
    {
        if (mLevel < globalTraceLevel) {
            return *this;
        }
        if (traceStream) {
            for(unsigned int i = 0; i < mNestingLevel; ++i) {
                *traceStream << mNestingPrefix;
            }
            QByteArray byteArray;
            QBuffer buffer(&byteArray);
            buffer.open(QIODevice::WriteOnly);
            QDebug(&buffer) << value;

            *traceStream << byteArray.data();
            if (mInsertSpaces) {
                *traceStream << " ";
            }
        }
        return *this;
    }
# endif
#else
    template<typename T>
    const Tracer& operator<<(const T&) const
    {
        return *this;
    }
    const Tracer& operator<<(ioFunctor) const
    {
        return *this;
    }
#endif

private:
    explicit Tracer(bool fixed = false, TraceLevel level = TraceLevel::trace, bool insertSpaces = true, bool insertEndl = true);
    void print(const char* marker, const std::string& trailer = "") const;

    bool mFixed;
    std::string mName;
    Timer mTimer;
    TraceLevel mLevel;
    bool mInsertSpaces;
    bool mInsertEndl;

    static std::string mNestingPrefix;
    static unsigned int mNestingLevel;
    static const char* mEntryMarker;
    static const char* mExitMarker;

    static TraceLevel globalTraceLevel;

    static std::ostream* traceStream;
};

Tracer trace(Tracer::TraceLevel level = Tracer::TraceLevel::trace, bool insertEndl = true);
Tracer notrace();
