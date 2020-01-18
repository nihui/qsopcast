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

#include <QObject>
#include <QProcess>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QByteArray>
#include <QSettings>

#include "channel_pplive.h"

ChannelPPLive::ChannelPPLive( QObject* parent ) :
    AbstractChannel( parent )
{
    m_sourcetype = PPLIVE;
    pplive = 0;
}

ChannelPPLive::~ChannelPPLive()
{
    stop();
}

QString ChannelPPLive::playerUrl()
{
    return QString( "http://127.0.0.1:1024/1.asf" );
}

void ChannelPPLive::launch()
{
    ///fork pplive process
    QSettings settings;
    QString pplivebin = settings.value( "/qsopcast/pplive_bin", "/usr/bin/xpplive" ).toString();
    QStringList args;
    args << m_url;
    pplive = new QProcess( this );
    pplive->start( pplivebin, args );
}

void ChannelPPLive::stop()
{
    if ( pplive ) {
        pplive->close();
        delete pplive;
        pplive = 0;
    }
}

ChannelListPPLive::ChannelListPPLive( QObject* parent ) :
    AbstractChannelList( parent )
{
    wget = 0;
}

void ChannelListPPLive::retrieveChannels()
{
    QString program = "wget";
    QSettings settings;
    QString channellisturl = settings.value( "/qsopcast/pplive_channelurl", "http://vnet.pplive.com/vnet2.xml" ).toString();
    QStringList args;
//     args << "http://list.pplive.com/zh-cn/xml/secure2.xml" << "-O" << "-";
// args << "http://list.pplive.com/zh-cn/xml/new.xml" << "-O" << "-";

    args << channellisturl << "-O" << "-";

//     args << "http://list.pplive.com/zh-cn/xml/default.xml" << "-O" << "-";
    wget = new QProcess( this );
    wget->start( program, args );
    connect( wget, SIGNAL( finished( int, QProcess::ExitStatus ) ), this, SLOT( parse() ) );
}

void ChannelListPPLive::parse()
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

    QHash<int, QString> groupnamehash;

    ///channel
    QDomNode channelnode = docElem.firstChild();
    while ( !channelnode.isNull() ) {
        if ( channelnode.isElement() && channelnode.nodeName() == "CatalogProperty" ) {
            ///group name from hash
            QDomNode catalognode = channelnode.firstChild();
            while ( !catalognode.isNull() ) {
                groupnamehash[ catalognode.firstChild().firstChild().nodeValue().toInt() ] = catalognode.lastChild().firstChild().nodeValue();
                catalognode = catalognode.nextSibling();
            }
        }
        else if ( channelnode.isElement() && channelnode.nodeName() == "channel" ) {
                ///element
                ChannelPPLive* channel = new ChannelPPLive( this );
                QString channelgroup;
                QDomNode namenode = channelnode.firstChild();
                while ( !namenode.isNull() ) {
                    if ( namenode.nodeName() == "ChannelName" )
                        channel->m_name = namenode.firstChild().nodeValue();
                    else if ( namenode.nodeName() == "CataLogId" ) {
                        int groupid = namenode.firstChild().nodeValue().toInt();
                        if ( groupnamehash.contains( groupid ) )
                            channelgroup = groupnamehash[ groupid ];
                        else
                            channelgroup = tr( "PPLive other channels" );
                    }
                    else if ( namenode.nodeName() == "Catalog" )
                        channelgroup = namenode.firstChild().nodeValue();
                    else if ( namenode.nodeName() == "Quatity" )
                        channel->m_quality = namenode.firstChild().nodeValue().toInt();
                    else if ( namenode.nodeName() == "PeerCount" )
                        channel->m_peercount = namenode.firstChild().nodeValue().toInt();
                    else if ( namenode.nodeName() == "Bitrate" )
                        channel->m_kbps = namenode.firstChild().nodeValue().toInt();
                    else if ( namenode.nodeName() == "PlayLink" )
                        channel->m_url = namenode.firstChild().nodeValue();
                    namenode = namenode.nextSibling();
                }
                channel->m_format = wmv;
                channels[ channelgroup ].append( channel );
        }
        channelnode = channelnode.nextSibling();
    }

    emit channellistRetrieved( channels );
}

#include "channel_pplive.moc"
   