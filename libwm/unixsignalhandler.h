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

#ifdef QT_VERSION

#include <QObject>

#include "singleton.h"

class QSocketNotifier;

#ifdef Q_OS_UNIX

class LIBWMSHARED_EXPORT UnixSignalHandler final : public QObject, public Singleton<UnixSignalHandler>
{
    Q_OBJECT
    friend class Singleton<UnixSignalHandler>;
public:
    void mapUnixSignal(int unixSignalNumber);

signals:
    void unixSignal(int unixSignalNumber);
    void mappingAdded(int unixSignalNumber);

private slots:
    void pipeActivated();

private:
    UnixSignalHandler();
    ~UnixSignalHandler();

    static void signalHandler(int signal);
    static int mPipeFds[2];

    static QSocketNotifier* mNotifier;
};

#endif // UNIX

#endif // QT_VERSION

