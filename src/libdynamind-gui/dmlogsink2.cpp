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
#include <dmlogsink2.h>
#include <QMessageBox>
#include "iostream"

using namespace DM;

OStreamLogSink2::OStreamLogSink2(std::ostream &ostream) : out(ostream) {
	mutex = new QMutex(QMutex::Recursive);
    printerror = false;
}

OStreamLogSink2::~OStreamLogSink2()
{
	delete mutex;
}

LogSink2 &OStreamLogSink2::operator<<(const std::string &string) {
    QMutexLocker locker(mutex);
    if(string.find("ERROR") == std::string::npos)
    {
        printerror = false;
        return *this;
    }
    if(!printerror)
        emit this->callError();
    printerror = true;
	return *this;
}

LogSink2 &OStreamLogSink2::operator<<(const char *string) {
	QMutexLocker locker(mutex);
	return *this;
}

LogSink2 &OStreamLogSink2::operator<<(int i) {
	QMutexLocker locker(mutex);
	return *this;
}


LogSink2 &OStreamLogSink2::operator<<(double f) {
	QMutexLocker locker(mutex);
	return *this;
}
LogSink2 &OStreamLogSink2::operator<<(LSEndl e) {
	QMutexLocker locker(mutex);
    return *this;
}

