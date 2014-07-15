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


/** @brief Interface class LogSink
*
* All messages send to the logger are directed to the LogSink
*/
class DM_HELPER_DLL_EXPORT LogSink2 : public QObject,public LogSink
{
Q_OBJECT
public:
    virtual ~LogSink2() {}
	virtual void close() = 0;
	/** @brief sends a string */
    virtual LogSink2 &operator<<(const std::string &string) = 0;
	/** @brief sends a char* */
    virtual LogSink2 &operator<<(const char *string) = 0;
	/** @brief sends a double */
    virtual LogSink2 &operator<<(double f) = 0;
	/** @brief sends end */
    virtual LogSink2 &operator<<(LSEndl) = 0;
};

/** @brief implements class LogSink
*
* can be used together with cout see Log for an example */
class DM_HELPER_DLL_EXPORT OStreamLogSink2 : public LogSink2{
Q_OBJECT
public:
    OStreamLogSink2(std::ostream &ostream);
    ~OStreamLogSink2();
	void close() {}
	/** @brief sends a string to ostream*/
    LogSink2 &operator<<(const std::string &string);
	/** @brief sends a char*  to ostream*/
    LogSink2 &operator<<(const char *string);
	/** @brief sends an int to ostream*/
    LogSink2 &operator<<(int i);
	/** @brief sends a double to ostream*/
    LogSink2 &operator<<(double f);
	/** @brief sends std::endl to ostream*/
    LogSink2 &operator<<(LSEndl e);
Q_SIGNALS:
    void callError();
private:
	std::ostream &out;
	QMutex *mutex;
    bool printerror;

};
}
#endif // LOGSINK2_H

