/*
    This file is part of qsopcast project.
    Copyright (C) 2005, 2006  Liu Di <liudidi@gmail.com>
    Copyright (C) 2007  Wei Lian <lianwei3@gmail.com>
    Copyright (C) 2009, 2010  Ni Hui <shuizhuyuanluo@126.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QApplication>
#include <QLocale>
#include <QString>
#include <QTranslator>

#include "qsopcast.h"

int main( int argc, char **argv )
{
    QApplication app( argc, argv );
    QCoreApplication::setOrganizationName( "Magic Group" );
    QCoreApplication::setOrganizationDomain( "magiclinux.org" );
    QCoreApplication::setApplicationName( "qsopcast" );

    /// choose language
    QTranslator translator;
    translator.load( QLocale().name().section( ".", 0, 0 ), "/usr/share/apps/qsopcast/" );
    app.installTranslator( &translator );

    QSopcast window;
    window.show();

    return app.exec();
}
