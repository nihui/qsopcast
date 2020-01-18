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

#include <stdlib.h>// for srand() and rand()

#include <QObject>
#include <QProcess>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QByteArray>
#include <QSignalMapper>
#include <QDir>
#include <QFile>
#include <QSettings>

#include "utils.h"

#include "channel_ppstream.h"

ChannelPPStream::ChannelPPStream( QObject* parent ) :
    AbstractChannel( parent )
{
    m_sourcetype = PPSTREAM;
    ppstream = 0;
}

ChannelPPStream::~ChannelPPStream()
{
    stop();
}

QString ChannelPPStream::playerUrl()
{
    return QString( "http://127.0.0.1:" ) + QString::number( outport );
}

void ChannelPPStream::launch()
{
    /// select a random unused outport number
    while ( 1 ) {
        static int r = 0;
        ++r;
        srand( time( NULL ) + r );
        outport = rand() % ( 65536 - 10000 ) + 10000; ////10000 to 65535
        int sock = connect_to_server( "127.0.0.1", outport );
        if ( sock == -1 )
            break;
        else if ( sock >= 0 )
            ::close( sock );
    }

    /// fork ppstream process
    QSettings settings;
    QString ppstreambin = settings.value( "/qsopcast/ppstream_bin", "/usr/bin/xpps" ).toString();
    QStringList args;
    args <<  "-p" << QString::number( outport ) << "-l" << m_url;
    ppstream = new QProcess( this );
    ppstream->setProcessChannelMode( QProcess::MergedChannels );
    connect( ppstream, SIGNAL( readyReadStandardOutput() ), this, SLOT( parseStdout() ) );
    ppstream->start( ppstreambin, args );
}

void ChannelPPStream::stop()
{
    if ( ppstream ) {
        ppstream->close();
        delete ppstream;
        ppstream = 0;
    }
}

void ChannelPPStream::parseStdout()
{
    while ( ppstream->canReadLine() ) {
        QString line = QString( ppstream->readLine() );
        QString statusmsg = line;
        emit channelStatusUpdated( statusmsg );
    }
}

ChannelListPPStream::ChannelListPPStream( QObject* parent ) :
    AbstractChannelList( parent )
{
    wget = 0;
    channelFilesCount = 0;
    signalMapper = new QSignalMapper( this );
    connect( signalMapper, SIGNAL( mapped( const QString& ) ),
             this, SLOT( parseSubGroup( const QString& ) ) );
    signalMapper2 = new QSignalMapper( this );
    connect( signalMapper2, SIGNAL( mapped( const QString& ) ),
             this, SLOT( parseChannel( const QString& ) ) );
    /// ensure the directory exists
    if ( !QDir( QDir::homePath() + "/.qsopcast/ppstream_cache/" ).exists() )
        QDir().mkpath( QDir::homePath() + "/.qsopcast/ppstream_cache/" );
}

ChannelListPPStream::~ChannelListPPStream()
{
    // clear all
    clearChannels();
    clearSubChannels();
}

void ChannelListPPStream::retrieveChannels()
{
    QString program = "wget";
    QSettings settings;
    QString channellisturl = settings.value( "/qsopcast/ppstream_channelurl", "http://playlist.pps.tv/classfortv.php" ).toString();
    QStringList args;
    args << channellisturl << "-O" << "-";

    wget = new QProcess( this );
    wget->start( program, args );
    connect( wget, SIGNAL( finished( int, QProcess::ExitStatus ) ), this, SLOT( parseGroup() ) );
}

void ChannelListPPStream::clearChannels()
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
}

void ChannelListPPStream::clearSubChannels()
{
    QHash< QString, QHash< QString, QList<AbstractChannel*> > >::iterator it_groupname = subchannels.begin();
    while ( it_groupname != subchannels.end() ) {
        QHash< QString, QList<AbstractChannel*> >::iterator it_subgroupname = it_groupname.value().begin();
        while ( it_subgroupname != it_groupname.value().end() ) {
            QList<AbstractChannel*>::const_iterator it_channel = it_subgroupname.value().constBegin();
            while ( it_channel != it_subgroupname.value().constEnd() ) {
                delete *it_channel;
                ++it_channel;
            }
            it_subgroupname.value().clear();
            ++it_subgroupname;
        }
        it_groupname.value().clear();
        ++it_groupname;
    }
    subchannels.clear();
}

void ChannelListPPStream::parseGroup()
{
    // clear all
    clearChannels();
    clearSubChannels();

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
    
    QDomNode groupclassnode = docElem.firstChild();
    while ( !groupclassnode.isNull() ) {
        if ( groupclassnode.isElement() && groupclassnode.nodeName() == "Class" ) {
            int groupid = -1;
            QString groupname;
            int grouptype = 0;
            int contentnum = 0;
            QDomNode namenode = groupclassnode.firstChild();
            while ( !namenode.isNull() ) {
                if ( namenode.nodeName() == "ID" )
                    groupid = namenode.firstChild().nodeValue().toInt();
                else if ( namenode.nodeName() == "Title" )
                    groupname = namenode.firstChild().nodeValue();
                else if ( namenode.nodeName() == "Type" )
                    grouptype = namenode.firstChild().nodeValue().toInt();
                else if ( namenode.nodeName() == "ContentNum" )
                    contentnum = namenode.firstChild().nodeValue().toInt();
                namenode = namenode.nextSibling();
            }
            if ( groupid != -1 ) {
                groupnamehash[ groupid ] = groupname;
                subgentry_t subgentry = { groupid, grouptype, contentnum };
                subgroupqueue.enqueue( subgentry );
            }
        }
        groupclassnode = groupclassnode.nextSibling();
    }

    retrieveSubGroupsSync();
}

void ChannelListPPStream::retrieveSubGroupsSync()
{
    if ( !subgroupqueue.isEmpty() ) {
        subgentry_t subgentry = subgroupqueue.dequeue();
        retrieveSubGroup( subgentry.groupid, subgentry.grouptype, subgentry.contentnum );
    }
    else {
        emit channellistRetrieveStatusUpdated( QString( tr( "Ready" ) ) );
        emit channellistRetrieved( channels );
        emit channellistRetrieved( subchannels );
    }
}

void ChannelListPPStream::retrieveSubGroup( int groupid, int grouptype, int contentnum )
{
    QString xmlurl = "http://playlist.pps.tv/subclassfortv.php?class=" + QString::number( groupid );
    QString program = "wget";
    const QString localcacheurl = QDir::homePath() + "/.qsopcast/ppstream_cache/" + QString::number( groupid ) + '_' + QString::number( grouptype ) + "_.xml";
    QStringList args;
    QFileInfo fileinfo = QFileInfo( localcacheurl );
    if ( !fileinfo.exists() || fileinfo.size() < 1 ) {
        args << xmlurl << "-O" << localcacheurl;
    }
    else {
        /// do not run wget program, use cache files directly
        emit channellistRetrieveStatusUpdated( QString( tr( "Reading PPStream channel list %1..." ) ).arg( groupnamehash[groupid] ) );
        parseSubGroup( localcacheurl );
        return;
    }

    emit channellistRetrieveStatusUpdated( QString( tr( "Downloading PPStream channel list %1..." ) ).arg( groupnamehash[groupid] ) );

    QProcess* wgetsub = new QProcess;
    wgetsub->start( program, args );

    signalMapper->setMapping( wgetsub, localcacheurl );
    connect( wgetsub, SIGNAL( finished( int, QProcess::ExitStatus ) ),
             signalMapper, SLOT( map() ) );
}

void ChannelListPPStream::parseSubGroup( const QString& localcacheurl )
{
    QProcess* sender = static_cast<QProcess*>(signalMapper->mapping( localcacheurl ));
    signalMapper->removeMappings( sender );
    delete sender;

    QFile file( localcacheurl );
    if ( !file.open( QIODevice::ReadOnly ) ) {
        return;
    }

    QDomDocument doc;
    if ( !doc.setContent( &file ) ) {
        file.close();
        return;
    }
    file.close();

    int groupid;
    int grouptype;
    {
        groupid = QFileInfo( localcacheurl ).fileName().section( '_', 0, 0 ).toInt();
        grouptype = QFileInfo( localcacheurl ).fileName().section( '_', 1, 1 ).toInt();
    }

    QDomElement docElem = doc.documentElement();

    QDomNode subclassnode = docElem.firstChild();
    while ( !subclassnode.isNull() ) {
        if ( subclassnode.isElement() && subclassnode.nodeName() == "SubClass" ) {
            int subgroupid = -1;
            QString subgroupname;
            int contentnum = 0;
            QDomNode namenode = subclassnode.firstChild();
            while ( !namenode.isNull() ) {
                if ( namenode.nodeName() == "SubID" )
                    subgroupid = namenode.firstChild().nodeValue().toInt();
                else if ( namenode.nodeName() == "Title" )
                    subgroupname = namenode.firstChild().nodeValue();
                else if ( namenode.nodeName() == "ContentNum" )
                    contentnum = namenode.firstChild().nodeValue().toInt();
                namenode = namenode.nextSibling();
            }
            if ( subgroupid != -1 ) {
                subgroupnamehash[ subgroupid ] = subgroupname;

                chentry_t entry = { groupid, subgroupid, grouptype, contentnum };
                channelqueue.enqueue( entry );
            }
        }
        subclassnode = subclassnode.nextSibling();
    }

    retrieveChannelsSync();
}

void ChannelListPPStream::retrieveChannelsSync()
{
    if ( !channelqueue.isEmpty() ) {
        chentry_t entry = channelqueue.dequeue();
        retrieveChannel( entry.groupid, entry.subgroupid, entry.grouptype, entry.contentnum );
    }
    else {
        retrieveSubGroupsSync();
    }
}

void ChannelListPPStream::retrieveChannel( int groupid, int subgroupid, int grouptype, int contentnum )
{
    /// http://playlist.pps.tv/channelsfortv.php?class=%d&page=%d
    QString churl = "http://playlist.pps.tv/channelsfortv.php?class=" + QString::number( groupid );

    /// http://playlist.pps.tv/channelsfortv.php?class=%d&subclass=%d&page=%d
    if ( grouptype == 0 )
        churl += "&subclass=" + QString::number( subgroupid );

    int pages = contentnum / 50 + 1;
    channelFilesCount = pages;
    int i = 1;
    for ( ; i<=pages; ++i ) {
        QString xmlurl = churl + "&page=" + QString::number( i );
        const QString localcacheurl = QDir::homePath() + "/.qsopcast/ppstream_cache/" + QString::number( groupid ) + '_' + QString::number( subgroupid ) + '_' + QString::number( grouptype ) + '_' + QString::number( i ) + "_.xml";
        QString program = "wget";
        QStringList args;
        QFileInfo fileinfo = QFileInfo( localcacheurl );
        if ( !fileinfo.exists() || fileinfo.size() < 1 ) {
            args << xmlurl << "-q"
                << "--ignore-length"
                << "--no-http-keep-alive"
                << "--timestamping"
                << "--timeout=3"
                << "--tries=0"
                << "-O" << localcacheurl;
        }
        /// WARNING  never call parseChannel directly, like the following codes
        /// those seem to fast this app, but actually not at all
        /// ppstream channel list files are too many to be handled together
        /// it will eat your cpu time and memory too much,
        /// so just let wget program run to relax your cpu and delay the memory use  --- nihui
//         else {
//            /// do not run wget program, use cache files directly
//             parseChannel( localcacheurl );
//             continue;
//         }

        QProcess* wgetch = new QProcess;
        wgetch->start( program, args );
        signalMapper2->setMapping( wgetch, localcacheurl );
        connect( wgetch, SIGNAL( finished( int, QProcess::ExitStatus ) ),
                 signalMapper2, SLOT( map() ) );
    }

}

void ChannelListPPStream::parseChannel( const QString& localcacheurl )
{
    QProcess* sender = static_cast<QProcess*>(signalMapper2->mapping( localcacheurl ));
    signalMapper2->removeMappings( sender );
    delete sender;

    QFile file( localcacheurl );
    if ( !file.open( QIODevice::ReadOnly ) ) {
        collectChannelFiles();
        return;
    }

    QDomDocument doc;
    if ( !doc.setContent( &file ) ) {
        file.close();
        collectChannelFiles();
        return;
    }
    file.close();

    int groupid;
    int subgroupid;
    int grouptype;
    {
        groupid = QFileInfo( localcacheurl ).fileName().section( '_', 0, 0 ).toInt();
        subgroupid = QFileInfo( localcacheurl ).fileName().section( '_', 1, 1 ).toInt();
        grouptype = QFileInfo( localcacheurl ).fileName().section( '_', 2, 2 ).toInt();
    }

    QDomElement docElem = doc.documentElement();

    QDomNode filmnode = docElem.firstChild();
    while ( !filmnode.isNull() ) {
        if ( filmnode.isElement() && filmnode.nodeName() == "Film" ) {
            QString filmname;
            bool with_volumns = false;
            QDomNode namenode = filmnode.firstChild();
            while ( !namenode.isNull() ) {
                if ( namenode.nodeName() == "Title" )
                    filmname = namenode.firstChild().nodeValue();
                else if ( namenode.nodeName() == "Down" ) {
                    QDomNode downnode = namenode.firstChild();
                    while ( !downnode.isNull() ) {
                        QString ppstreamaddress;
                        QString mediaformat;
                        int rate = 0;
                        int volumn = 1;
                        if ( downnode.isElement() && downnode.nodeName() == "Url" ) {
                            QDomNode urlnode = downnode.firstChild();
                            while ( !urlnode.isNull() ) {
                                if ( urlnode.nodeName() == "CI" )
                                    volumn = urlnode.firstChild().nodeValue().toInt();
                                else if ( urlnode.nodeName() == "DUrl" )
                                    ppstreamaddress = urlnode.firstChild().nodeValue();
                                else if ( urlnode.nodeName() == "Form" )
                                    mediaformat = urlnode.firstChild().nodeValue();
                                else if ( urlnode.nodeName() == "Rate" )
                                    rate = urlnode.firstChild().nodeValue().toInt();
                                urlnode = urlnode.nextSibling();
                            }
                        }
                        ChannelPPStream* channel = new ChannelPPStream( this );
                        channel->m_name = filmname;
                        channel->m_kbps = rate;
                        channel->m_url = ppstreamaddress;
                        channel->m_format = (mediaformat=="wmv")? wmv : rmvb;
                        QString groupname = groupnamehash[groupid];
                        if ( grouptype == 0 ) {
                            /// group -> subgroup -> channel
                            QString subgroupname = subgroupnamehash[subgroupid];
                            subchannels[ groupname ][ subgroupname ].append( channel );
                        }
                        else if ( with_volumns ) {
                            channel->m_name = filmname + ' ' + QString::number( volumn );
                            /// group -> channel -> volumn, volumms regard as sub grouping entries
                            subchannels[ groupname ][ filmname ].append( channel );
                        }
                        else if ( !downnode.nextSibling().isNull() ) {
                            with_volumns = true;
                            channel->m_name = filmname + ' ' + QString::number( volumn );
                            /// group -> channel -> volumn, volumms regard as sub grouping entries
                            subchannels[ groupname ][ filmname ].append( channel );
                        } else {
                            /// group -> channel
                            channels[ groupname ].append( channel );
                        }
                        downnode = downnode.nextSibling();
                    }
                }
                namenode = namenode.nextSibling();
            }
        }
        filmnode = filmnode.nextSibling();
    }

    collectChannelFiles();
}

void ChannelListPPStream::collectChannelFiles()
{
    static int collectedFileCount = 0;
    ++collectedFileCount;
    if ( collectedFileCount == channelFilesCount ) {
        collectedFileCount = 0;
        channelFilesCount = 0;
        retrieveChannelsSync();
    }
}

#include "channel_ppstream.moc"
