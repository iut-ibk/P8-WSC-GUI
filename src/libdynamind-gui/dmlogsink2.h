/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011  Christian Urich

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */
#ifndef DMLOGSINK2_H
#define DMLOGSINK2_H

#include <string>
#include <ostream>
#include <dmcompilersettings.h>
#include <QMutex>
#include <QMutexLocker>
#include <dmlogsink.h>
#include "QWidget"

namespace DM {

/** @brief implements class LogSink
*
* can be used together with cout see Log for an example */
class DM_HELPER_DLL_EXPORT OStreamLogSink2 : public QObject, public LogSink{
Q_OBJECT
public:
    OStreamLogSink2(std::ostream &ostream);
    ~OStreamLogSink2();
    void close() {}
    /** @brief sends a string to ostream*/
    LogSink &operator<<(const std::string &string);
    /** @brief sends a char*  to ostream*/
    LogSink &operator<<(const char *string);
    /** @brief sends an int to ostream*/
    LogSink &operator<<(int i);
    /** @brief sends a double to ostream*/
    LogSink &operator<<(double f);
    /** @brief sends std::endl to ostream*/
    LogSink &operator<<(LSEndl e);
Q_SIGNALS:
    void callError();
private:
    std::ostream &out;
    QMutex *mutex;
    bool printerror;

};
}
#endif // LOGSINK2_H

