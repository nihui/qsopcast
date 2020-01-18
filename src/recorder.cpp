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

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <QDir>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QSocketNotifier>
#include <QDateTime>
#include <QTimer>
#include <QSettings>

#include "utils.h"
#include "recorder.h"

Recorder::Recorder( QObject* parent ):
        QObject( parent )
{
    snrecord = 0;
    recordfp = 0;
    launchtimer = new QTimer( this );
    launchtimer->setSingleShot( true );
    connect( launchtimer, SIGNAL( timeout() ), this, SLOT( startRecorder() ) );

    datetimestr = QDateTime::currentDateTime().toString();
}

Recorder::~Recorder()
{
    stopRecorder();
}

void Recorder::setUrl( const QString& url )
{
    /// url format:  http://127.0.0.1:12345
    /// extract to "127.0.0.1" and "12345"
    QStringList tmp = url.section( "http://", -1 ).split(':');
    address = tmp[0];
    port = tmp[1].toInt();
}

void Recorder::setChannelName( const QString& name )
{
    channelname = name;
}

void Recorder::startRecorder()
{
    ///handle stream
    int sockfd = connect_to_server( qPrintable( address ), port );
    if ( sockfd < 0 ) {
        ///restart
        launchtimer->start( 2000 );
        return;
    }
    ::write( sockfd, "1",::strlen( "1" ) + 1 );

    ///parse header
    /// WARNING: parse header action only apply to sopcast channel
    char buf[19];
    if ( ::recv( sockfd, buf, sizeof( buf ), 0 ) != sizeof( buf ) ) {
        fprintf( stderr, "err while parsing head\n" );
        ::close( sockfd );
        return;
    }

    ///testing data availibility
    int nread;
    ::ioctl( sockfd, FIONREAD, &nread );
    if ( nread <= 0 ) {
        ::close( sockfd );
        ////
        if ( nread < 0 )
            perror( "ioctl get nread of socket" );
        ///restart
        launchtimer->start( 2000 );
        return;
    }

    ///data availible
    QSettings settings;
    QString data_path = settings.value( "/qsopcast/recorddir", QDir::homePath() + "/SOPCAST/" ).toString();
    QDir dir( data_path );
    if ( !dir.exists() ) {
        if ( !dir.mkdir( data_path ) ) {
            perror( "failed to create record path" );
            ::close( sockfd );
            ///buttonrecord->setOn( false );
            return;
        }
    }
    data_path += channelname.replace( QRegExp( "\\/+" ), "#" ); ///eliminate harmful character
    data_path += ' ';
    data_path += datetimestr;
    data_path += ".asf";
    recordfp = new QFile( this );
    recordfp->setFileName( data_path );
    if ( !recordfp->open( QIODevice::WriteOnly ) ) {
        perror( "record file name is invalid" );
        ::close( sockfd );
        ///buttonrecord->setOn( false );
        return;
    }
    if ( snrecord ) {
        fprintf( stderr, "QSocketNotifier exist!\n" );
        return;
    }

    snrecord = new QSocketNotifier( sockfd, QSocketNotifier::Read, this );
    connect( snrecord, SIGNAL( activated( int ) ), SLOT( streamReady( int ) ) );
}

void Recorder::stopRecorder()
{
    if ( snrecord ) {
        recordfp->close();
        ::close( snrecord->socket() );
        delete snrecord;
        snrecord = 0;
    }
    delete recordfp;
    recordfp = 0;
}

void Recorder::streamReady( int fd )
{
    int ret;
    char buff[1024];
    ret = ::read( fd, buff, sizeof( buff ) );
    if ( ret <= 0 ) {
        if ( ret < 0 )
            perror( "record socketnotifier" );
        recordfp->close();
        ::close( fd );
        delete snrecord;
        snrecord = 0;

        ///restart record
        launchtimer->start( 2000 );
        return;
    }
    ::write( recordfp->handle(), buff, ret );
}

#include "recorder.moc"
