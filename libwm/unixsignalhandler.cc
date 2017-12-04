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

#include "unixsignalhandler.h"

#include <unistd.h>
#include <signal.h>

#ifdef QT_VERSION

#include <QDebug>
#include <QSocketNotifier>

#define NO_TRACE
#include "tracer.h"

#define PIPE_READ 0
#define PIPE_WRITE 1

#ifdef Q_OS_UNIX

QSocketNotifier* UnixSignalHandler::mNotifier = nullptr;
int UnixSignalHandler::mPipeFds[2] = {-1, -1};

void UnixSignalHandler::signalHandler(int signal)
{
    // only call async-signal-safe functions from within a unix-signal handler
    Q_ASSERT(mPipeFds[PIPE_WRITE] >= 0);

    if (write(UnixSignalHandler::mPipeFds[PIPE_WRITE], &signal, sizeof(int)) < 0) {
        perror("Can't write to pipe");
    }
}

UnixSignalHandler::UnixSignalHandler()
{
    TRACE;
    if ((mPipeFds[0] < 0) || (mPipeFds[1] < 0)) {
        Trace() << "create pipe";
        if (pipe(mPipeFds) < 0) {
            qFatal("Can't create pipe: %s", strerror(errno));
        }
    }
    Q_ASSERT(mPipeFds[PIPE_READ] >= 0);

    if (!mNotifier) {
        mNotifier = new QSocketNotifier(mPipeFds[PIPE_READ],
                                        QSocketNotifier::Read, this);
        connect(mNotifier, &QSocketNotifier::activated,
                this, &UnixSignalHandler::pipeActivated);
    }
}

UnixSignalHandler::~UnixSignalHandler()
{
    TRACE;
    if (mNotifier) {
        delete mNotifier;
        mNotifier = nullptr;
    }
    if (mPipeFds[0] >= 0) {
        close(mPipeFds[0]);
        mPipeFds[0] = -1;
    }
    if (mPipeFds[1] >= 0) {
        close(mPipeFds[1]);
        mPipeFds[1] = -1;
    }
}

void UnixSignalHandler::mapUnixSignal(int unixSignalNumber)
{
    TRACE;
    Q_ASSERT(unixSignalNumber > 0);
    Q_ASSERT(unixSignalNumber < SIGUNUSED);

    struct sigaction signalAction;
    struct sigaction oldSignalAction;

    memset(&signalAction, 0, sizeof(signalAction));

    signalAction.sa_handler = signalHandler;
    sigemptyset(&signalAction.sa_mask);
    signalAction.sa_flags |= SA_RESTART;

    if (sigaction(unixSignalNumber, &signalAction, &oldSignalAction) < 0) {
        qFatal("Can't install signal handler for signal %d: %s", unixSignalNumber,
               strerror(errno));
    }
    if (oldSignalAction.sa_handler) {
        qWarning() << "overriding old signal handler";
    }
    emit mappingAdded(unixSignalNumber);
}

void UnixSignalHandler::pipeActivated()
{
    TRACE;
    Q_ASSERT(mPipeFds[PIPE_READ] >= 0);

    int signal = -1;
    if (read(mPipeFds[PIPE_READ], &signal, sizeof(int)) < 0) {
        qWarning() << "Can't read from pipe";
    }
    if (signal >= 0) {
        emit unixSignal(signal);
    }
    else {
        qWarning() << "Wrong signal number read";
    }
}

#endif

#endif
