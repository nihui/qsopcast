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
#include <QApplication>
#include <QString>
#include <QStringList>

#include "channel_pplive.h"
#include "channel_ppstream.h"
#include "channel_sopcast.h"

#include "channel_argument.h"

ChannelListArgument* ChannelListArgument::m_self = 0;

ChannelListArgument* ChannelListArgument::self()
{
    if ( !m_self )
        m_self = new ChannelListArgument;
    return m_self;
}

ChannelListArgument::ChannelListArgument( QObject* parent ) :
    AbstractChannelList( parent )
{
}

ChannelListArgument::~ChannelListArgument()
{
    m_self = 0;
}

void ChannelListArgument::retrieveChannels()
{
    QStringList args = QApplication::arguments();
    QString channelurl;
    for ( int i=1; i<args.count(); ++i ) {
        channelurl = args[i];
        AbstractChannel* channel;
        /// guess the channel source type
        if ( channelurl.startsWith( "synacast://" ) )
            channel = new ChannelPPLive( this );
        else if ( channelurl.startsWith( "pps://" ) )
            channel = new ChannelPPStream( this );
        else if ( channelurl.startsWith( "sop://" ) )
            channel = new ChannelSopcast( this );
        else
            continue;// NOTE: can not decide it, throw this channel  ;)
        /// guess the channel media format
        if ( channelurl.endsWith( ".wmv" ) || channelurl.endsWith( ".asf" ) )
            channel->m_format = wmv;
        else if ( channelurl.endsWith( ".rmvb" ) || channelurl.endsWith( ".rm" ) )
            channel->m_format = rmvb;
        else if ( channelurl.endsWith( ".mp3" ) )
            channel->m_format = mp3;
        else
            channel->m_format = wmv;// NOTE: hmm... if wmv is good here?
        QString argindexstr = QString::number( i ) + ' ';
        channel->m_name = argindexstr + channelurl;
        channel->m_url = channelurl;
        channel->m_kbps = 0;/// do not record it
        /// FIXME: bookmark it, so we will not lose it if we restart app ?
        /// channel->m_sourcetype = (source_t)( channel->m_sourcetype | BOOKMARK );
        channels.append( channel );
    }

    if ( channels.count() > 0 ) {
        QHash< QString, QList<AbstractChannel*> > hashchannels;
        hashchannels[ tr( "My Arguments" ) ] = channels;
        emit channellistRetrieved( hashchannels );
    }
}

#include "channel_argument.moc"
