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

#include <stdio.h>// for fprintf()
#include <stdlib.h>// for srand() and rand()

#include <QObject>
#include <QBuffer>
#include <QProcess>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QByteArray>
#include <QSettings>
#include <QSocketNotifier>
#include <QTimer>

#include "utils.h"

#include "channel_sopcast.h"

ChannelSopcast::ChannelSopcast( QObject* parent ) :
    AbstractChannel( parent )
{
    m_sourcetype = SOPCAST;
    sop = 0;
    snstatus = 0;
    echotimer = 0;
}

ChannelSopcast::~ChannelSopcast()
{
    stop();
}

QString ChannelSopcast::playerUrl()
{
    return QString( "http://127.0.0.1:" ) + QString::number( outport );
}

void ChannelSopcast::launch()
{
    if ( echotimer == 0 ) {
        echotimer = new QTimer( this );
        connect( echotimer, SIGNAL( timeout() ), this, SLOT( echo() ) );
    }
    {
        ///select random unused inport and outport number
        int sock;
        static int r = 0;
        while ( 1 ) {
            ++r;
            srand( time( NULL ) + r );
            inport = rand() % ( 65536 - 10000 ) + 10000;/// from 10000 to 65535
            sock = connect_to_server( "127.0.0.1", inport );
            if ( sock == -1 )
                break;
            else if ( sock >= 0 )
                ::close( sock );
        }
        while ( 1 ) {
            ++r;
            srand( time( NULL ) + r );
            outport = rand() % ( 65536 - 10000 ) + 10000;
            sock = connect_to_server( "127.0.0.1", outport );
            if ( sock == -1 && outport != inport )
                break;
            else if ( sock >= 0 )
                ::close( sock );
        }
    }

    ///fork sop process
    QSettings settings;
    QString sopbin = settings.value( "/qsopcast/sopcast_bin", "/usr/bin/sp-sc-auth" ).toString();
    QStringList args;
    if ( settings.value( "/qsopcast/sopcast_forcetcp", false ).toBool() )
        args << "-T";
    if ( settings.value( "/qsopcast/sopcast_useexittime", false ).toBool() ) {
        int exittime = settings.value( "/qsopcast/sopcast_exittime", "1000" ).toInt();
        args << "-t" << QString::number( exittime );
    }
    if ( settings.value( "/qsopcast/sopcast_useauth", false ).toBool() ) {
        QString username = settings.value( "/qsopcast/sopcast_username", "root" ).toString();
        QString password = settings.value( "/qsopcast/sopcast_password", "root" ).toString();
        args << "-u" << username + ':' + password;
    }
    args << m_url << QString::number( inport ) << QString::number( outport );
    sop = new QProcess( this );
    sop->start( sopbin, args );
    connect( sop, SIGNAL( readyReadStandardOutput() ), this, SLOT( parseStdout() ) );
}

void ChannelSopcast::stop()
{
    if ( sop ) {
        sop->close();
        delete sop;
        sop = 0;
    }
    if ( echotimer ) {
        /// stop timer first, otherwise socket fd will be unavailable for write
        echotimer->stop();
        disconnect( echotimer, SIGNAL( timeout() ), this, SLOT( echo() ) );
        delete echotimer;
        echotimer = 0;
    }
    delete snstatus;
    snstatus = 0;
}

void ChannelSopcast::parseStdout()
{
    QString str = QString( sop->readAllStandardOutput() );

    if ( snstatus == 0 ) {
        if ( str.contains( "hook_sc" ) || str.contains( "downloadRate=" ) ) {
            //create socket
            int sockfd = connect_to_server( "127.0.0.1", outport );
            if ( sockfd < 0 ) {
                fprintf( stderr, "failed to create socket, return err %d\n", sockfd );
                return;
            }
            ///bind QSocketNotifier to socket
            snstatus = new QSocketNotifier( sockfd, QSocketNotifier::Read, this );
            connect( snstatus, SIGNAL( activated( int ) ), this, SLOT( streamReady( int ) ) );
            write( sockfd, "state\ns\n", sizeof( "state\ns\n" ) );
            write( sockfd, "state\ns\n", sizeof( "state\ns\n" ) );
        }
        else if ( str.contains( "detect MTU" ) ) {
            /// update message
            QString message = tr( "Waiting..." );
            emit channelStatusUpdated( message );
        }
    }
}

void ChannelSopcast::streamReady( int fd )
{
    char buff[64];
    memset( buff, 0, sizeof( buff ) );

    int size = sizeof( buff );
    int red = 0;
    int ret;
    while ( 1 ) {
        ret =::read( fd, buff + red, size );
        if ( ret <= 0 ) { ///server shutdown
            if ( ret < 0 )
                perror( "status socketnotifier" );
            //close socket and delete socket noitifier
            ::close( fd );
            delete snstatus;
            snstatus = 0;
            ///stop timer
            echotimer->stop();
            ///change statusbar interface
            ///update message
            QString message = tr( "Connecting..." );
            emit channelStatusUpdated( message );
            return;
        }
        if ( strstr( buff, "\n" ) )
            break;

        red += ret;
        size -= ret;
    }

    if ( buff[0] != 10 ) {
        /// format == buf, upload, download, upload speed, download speed, peers
        /// sample buff == "99   35555   12322   177     2772     14"
        /// TODO: optimize the following codes
        char *savept;
        ::strtok_r( buff, "\n", &savept );

        int value[6];
        value[0] = atoi( strtok_r( buff, " ", &savept ) );
        for ( int i = 1; i < 6; i++ ) {
            value[i] = atoi( strtok_r( NULL, " ", &savept ) );
            if ( i < 5 )
                value[i] = value[i] / 1024;
        }

        static const char format[][10] = { "%d%%", "ur=%dk", "dr=%dk", "us=%dk", "ds=%dk", "peers=%d" };

        QStringList arraymessage;
        for ( int i = 0; i < 6; i++ ) {
            sprintf( buff, format[i], value[i] );
            arraymessage.append( QString( buff ) );
        }
        ///show arraymessage on statusbar
        emit channelStatusUpdated( arraymessage.join( "   " ) );
    }
//         else
//             printf( "buff=10\n" );

    ///singleshot
    echotimer->start( 1000 );
    echotimer->setSingleShot( true );
}

void ChannelSopcast::echo()
{
    write( snstatus->socket(), "s\n", sizeof( "s\n" ) );
    echotimer->start( 1000 );
    echotimer->setSingleShot( true );
}

ChannelListSopcast::ChannelListSopcast( QObject* parent ) :
    AbstractChannelList( parent )
{
    wget = 0;
}

void ChannelListSopcast::retrieveChannels()
{
    QString program = "wget";
    QSettings settings;
    QString channellisturl = settings.value( "/qsopcast/sopcast_channelurl", "http://channel.sopcast.com/gchlxml" ).toString();
    QStringList args;
    args << channellisturl << "-O" << "-";
    wget = new QProcess( this );
    wget->start( program, args );
    connect( wget, SIGNAL( finished( int, QProcess::ExitStatus ) ), this, SLOT( parse() ) );
}

void ChannelListSopcast::parse()
{
    // clear all
    QHash< QString, QList<AbstractChannel*> >::iterator it_groupname = channels.begin();
    while ( it_groupname != channels.end() ) {
        QList<AbstractChannel*>::const_iterator it_channel = it_groupname.value().constBegin();
        while ( it_channel != it_groupname.value().constEnd() ) {
            delete *it_channel;
            ++it_channel;
        }
        it_groupname.value().clear();
        ++it_groupname;
    }
    channels.clear();

    QByteArray out = wget->readAllStandardOutput();

    QDomDocument doc;
    doc.setContent( out );
    QDomElement docElem = doc.documentElement();

    ///group
    QDomNode groupnode = docElem.firstChild();
    while ( !groupnode.isNull() ) {
        if ( groupnode.isElement() && groupnode.nodeName() == "group" ) {
            ///channel
            QDomNode channelnode = groupnode.firstChild();
            while ( !channelnode.isNull() ) {
                if ( channelnode.isElement() && channelnode.nodeName() == "channel" ) {
                    ///element
                    ChannelSopcast* channel = new ChannelSopcast( this );
                    QDomNode namenode = channelnode.firstChild();
                    while ( !namenode.isNull() ) {
                        if ( namenode.nodeName() == "name" ) {
                            QString channelname = namenode.attributes().namedItem( "cn" ).nodeValue();
                            if ( channelname == QString::QString() ) {
                                channelname = namenode.attributes().namedItem( "en" ).nodeValue();
                                if ( channelname == QString::QString() )
                                    channelname = namenode.firstChild().nodeValue();
                            }
                            channel->m_name = channelname;
                        }
                        else if ( namenode.nodeName() == "status" )
                            channel->m_status = namenode.firstChild().nodeValue().toInt();
                        else if ( namenode.nodeName() == "region" )
                            channel->m_region = namenode.firstChild().nodeValue();
                        else if ( namenode.nodeName() == "class" )
                            channel->m_class = namenode.firstChild().nodeValue().toInt();
                        else if ( namenode.nodeName() == "user_count" )
                            channel->m_usercount = namenode.firstChild().nodeValue().toInt();
                        else if ( namenode.nodeName() == "sn" )
                            channel->m_sn = namenode.firstChild().nodeValue().toInt();
                        else if ( namenode.nodeName() == "visit_count" )
                            channel->m_visitcount = namenode.firstChild().nodeValue().toInt();
                        else if ( namenode.nodeName() == "start_from" )
                            channel->m_startfrom = namenode.firstChild().nodeValue();
                        else if ( namenode.nodeName() == "stream_type" )
                            channel->m_format = (namenode.firstChild().nodeValue()=="wmv")? wmv: rmvb;
                        else if ( namenode.nodeName() == "kbps" )
                            channel->m_kbps = namenode.firstChild().nodeValue().toInt();
                        else if ( namenode.nodeName() == "qs" )
                            channel->m_qs = namenode.firstChild().nodeValue().toInt();
                        else if ( namenode.nodeName() == "qc" )
                            channel->m_qc = namenode.firstChild().nodeValue().toInt();
                        else if ( namenode.nodeName() == "sop_address" )
                            channel->m_url = namenode.firstChild().firstChild().nodeValue();
                        else if ( namenode.nodeName() == "description" )
                            channel->m_description = namenode.firstChild().nodeValue();
                        namenode = namenode.nextSibling();
                    }

                    /// append channel
                    QString groupname = groupnode.attributes().namedItem( "cn" ).nodeValue();
                    if ( groupname == QString::QString() ) {
                        groupname = groupnode.attributes().namedItem( "en" ).nodeValue();
                        if ( groupname == QString::QString() )
                            groupname = groupnode.firstChild().nodeValue();
                    }
                    channels[ groupname ].append( channel );
                }
                ///channel
                channelnode = channelnode.nextSibling();
            }
        }
        ///group
        groupnode = groupnode.nextSibling();
    }

    emit channellistRetrieved( channels );
}

#include "channel_sopcast.moc"
