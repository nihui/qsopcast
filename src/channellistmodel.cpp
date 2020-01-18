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

#include <QStandardItemModel>
#include <QStandardItem>
#include <QString>
#include <QStringList>
#include <QBrush>
#include <QColor>

#include "abstractchannel.h"
#include "channel_argument.h"
#include "channel_bookmark.h"
#include "channel_history.h"
#include "channel_pplive.h"
#include "channel_ppstream.h"
#include "channel_sopcast.h"
#include "mystatusbar.h"

#include "channellistmodel.h"

ChannelListModel* ChannelListModel::m_self = 0;

ChannelListModel* ChannelListModel::self()
{
    if ( !m_self )
        m_self = new ChannelListModel;
    return m_self;
}

ChannelListModel::~ChannelListModel()
{
    m_self = 0;
}

ChannelListModel::ChannelListModel( QObject* parent ) :
    QStandardItemModel( parent )
{
    statusbar = MyStatusBar::self();

    QStringList headerstrs;
    headerstrs << tr( "Channel" );
    setHorizontalHeaderLabels( headerstrs );

    sop = new ChannelListSopcast( this );
    connect( sop, SIGNAL( channellistRetrieved( const QHash< QString, QList<AbstractChannel*> >& ) ),
             this, SLOT( addChannelList( const QHash< QString, QList<AbstractChannel*> >& ) ) );
    pplive = new ChannelListPPLive( this );
    connect( pplive, SIGNAL( channellistRetrieved( const QHash< QString, QList<AbstractChannel*> >& ) ),
             this, SLOT( addChannelList( const QHash< QString, QList<AbstractChannel*> >& ) ) );
    ppstream = new ChannelListPPStream( this );
    connect( ppstream, SIGNAL( channellistRetrieved( const QHash< QString, QList<AbstractChannel*> >& ) ),
             this, SLOT( addChannelList( const QHash< QString, QList<AbstractChannel*> >& ) ) );
    connect( ppstream, SIGNAL( channellistRetrieved( const QHash< QString, QHash< QString, QList<AbstractChannel*> > >& ) ),
             this, SLOT( addChannelList( const QHash< QString, QHash< QString, QList<AbstractChannel*> > >& ) ) );
    connect( ppstream, SIGNAL( channellistRetrieveStatusUpdated( const QString& ) ),
             statusbar, SLOT( showMessage( const QString& ) ) );
    bookmark = ChannelListBookmark::self();
    connect( bookmark, SIGNAL( channellistRetrieved( const QHash< QString, QList<AbstractChannel*> >& ) ),
             this, SLOT( addChannelList( const QHash< QString, QList<AbstractChannel*> >& ) ) );
    history = ChannelListHistory::self();
    connect( history, SIGNAL( channellistRetrieved( const QHash< QString, QList<AbstractChannel*> >& ) ),
             this, SLOT( addChannelList( const QHash< QString, QList<AbstractChannel*> >& ) ) );
    argument = ChannelListArgument::self();
    connect( argument, SIGNAL( channellistRetrieved( const QHash< QString, QList<AbstractChannel*> >& ) ),
             this, SLOT( addChannelList( const QHash< QString, QList<AbstractChannel*> >& ) ) );
}

void ChannelListModel::refreshChannelList()
{
    QStringList headerstrs;
    headerstrs << tr( "Channel" );
    clear();
    setHorizontalHeaderLabels( headerstrs );

    sop->retrieveChannels();
    pplive->retrieveChannels();
    ppstream->retrieveChannels();
    bookmark->retrieveChannels();
    history->retrieveChannels();
    argument->retrieveChannels();
}

void ChannelListModel::addChannelList( const QHash< QString, QList<AbstractChannel*> >& data )
{
    QStandardItem* rootitem = invisibleRootItem();

    QHash< QString, QList<AbstractChannel*> >::const_iterator it_groupname = data.constBegin();
    while ( it_groupname != data.constEnd() ) {
        QStandardItem* groupitem = new QStandardItem( it_groupname.key() );

        QList<AbstractChannel*>::const_iterator it_channel = it_groupname.value().constBegin();
        while ( it_channel != it_groupname.value().constEnd() ) {
            QStandardItem* channelitem = new QStandardItem( (*it_channel)->m_name );
            // NOTE: just store the point, no need to subclass channelitem from qstandarditem
            channelitem->setData( QVariant::fromValue( static_cast<QObject*>(*it_channel) ) );
            int kbps = (*it_channel)->m_kbps;
            int blue = 255 - kbps/2;
            int green = 0;
            if ( blue < 0 ) {green = -blue/3;if(green>255)green=255;blue = 0;}
            channelitem->setForeground( QBrush( QColor( 0, green, blue ) ) );

            groupitem->appendRow( channelitem );
            ++it_channel;
        }

        rootitem->appendRow( groupitem );
        ++it_groupname;
    }

    countFinishedLists();
}

void ChannelListModel::addChannelList( const QHash< QString, QHash< QString, QList<AbstractChannel*> > >& data )
{
    QStandardItem* rootitem = invisibleRootItem();
    
    QHash< QString, QHash< QString, QList<AbstractChannel*> > >::const_iterator it_groupname = data.constBegin();
    while ( it_groupname != data.constEnd() ) {
        QStandardItem* groupitem = new QStandardItem( it_groupname.key() );
        QHash< QString, QList<AbstractChannel*> >::const_iterator it_subgroupname = it_groupname.value().constBegin();
        while ( it_subgroupname != it_groupname.value().constEnd() ) {
            QStandardItem* subgroupitem = new QStandardItem( it_subgroupname.key() );
            
            QList<AbstractChannel*>::const_iterator it_channel = it_subgroupname.value().constBegin();
            while ( it_channel != it_subgroupname.value().constEnd() ) {
                QStandardItem* channelitem = new QStandardItem( (*it_channel)->m_name );
                // NOTE: just store the point, no need to subclass channelitem from qstandarditem
                channelitem->setData( QVariant::fromValue( static_cast<QObject*>(*it_channel) ) );
                int kbps = (*it_channel)->m_kbps;
                int blue = 255 - kbps/2;
                int green = 0;
                if ( blue < 0 ) {green = -blue/3;if(green>255)green=255;blue = 0;}
                channelitem->setForeground( QBrush( QColor( 0, green, blue ) ) );

                subgroupitem->appendRow( channelitem );
                ++it_channel;
            }
            groupitem->appendRow( subgroupitem );
            ++it_subgroupname;
        }
        rootitem->appendRow( groupitem );
        ++it_groupname;
    }
    
    countFinishedLists();
}

void ChannelListModel::countFinishedLists() const
{
    static const int lists = 6;/// NOTE sync the number with the channellist signals
    static int count = 0;
    ++count;
    if ( count == lists ) {
        count = 0;
        emit channelListRefreshed();/// notify the channel button to toggle
    }
}

#include "channellistmodel.moc"
