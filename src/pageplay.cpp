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

#include <QModelIndex>
#include <QStandardItem>
#include <QWidget>
#include <QHBoxLayout>
#include <QString>
#include <QPushButton>

#include "abstractchannel.h"
#include "channellistmodel.h"
#include "channellistview.h"
#include "channel_history.h"
#include "channel_sopcast.h"
#include "channel_pplive.h"
#include "channel_ppstream.h"
#include "tabwidget.h"
#include "playfork.h"
#include "mystatusbar.h"

#include "pageplay.h"

PagePlay::PagePlay( QWidget* parent ):
        QWidget( parent )
{
    QHBoxLayout* layout = new QHBoxLayout;
    // shrink the height and width
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 2 );
    setLayout( layout );
    ////row
    buttonlaunch = new QPushButton( tr( "Launch" ) );
    buttonlaunch->setCheckable( true );
    connect( buttonlaunch, SIGNAL( toggled( bool ) ), this, SLOT( onButtonLaunchClicked( bool ) ) );
    layout->addWidget( buttonlaunch );

    buttonplayer = new QPushButton( tr( "Player" ) );
    buttonplayer->setCheckable( true );
    connect( buttonplayer, SIGNAL( toggled( bool ) ), this, SLOT( onButtonPlayerToggled( bool ) ) );
    layout->addWidget( buttonplayer );

    buttonclose = new QPushButton( tr( "x" ) );
    layout->addWidget( buttonclose );

    channel = 0;
    playfork = 0;
    statusbar = MyStatusBar::self();
}

PagePlay::~PagePlay()
{
    if ( channel ) {
        channel->stop();
        channel = 0;
    }
    if ( playfork ) {
        playfork->killPlayer();
        delete playfork;
        playfork = 0;
    }
}

void PagePlay::onButtonPlayerToggled( bool t )
{
    if ( t ) {
        if ( channel ) {
            playfork = new PlayFork( this );
            playfork->forkPlayer( channel->playerUrl(), channel->m_format );
        }
    }
    else {
        if ( playfork ) {
            playfork->killPlayer();
            delete playfork;
            playfork = 0;
        }
    }
}

void PagePlay::onButtonLaunchClicked( bool t )
{
    if ( t ) {
        QModelIndex index = ChannelListView::self()->currentIndex();
        // check if it is a null channel or a group entry
        if ( index.isValid() && !ChannelListView::self()->model()->hasChildren( index ) ) {
            QStandardItem* item = ChannelListModel::self()->itemFromIndex( index );
            channel = static_cast<AbstractChannel*>( item->data().value<QObject*>() );
            TabWidget::self()->setTabText( TabWidget::self()->currentIndex(), channel->m_name );
            connect( channel, SIGNAL( channelStatusUpdated( const QString& ) ),
                     statusbar, SLOT( showMessage( const QString& ) ) );
            channel->launch();
            /// append to history records
            onAppendToHistory();
        }
    }
    else {
        buttonplayer->setChecked( false );
        TabWidget::self()->setTabText( TabWidget::self()->currentIndex(), tr( "play" ) );
        if ( channel ) {
            channel->stop();
            disconnect( channel, SIGNAL( channelStatusUpdated( const QString& ) ),
                     statusbar, SLOT( showMessage( const QString& ) ) );
            channel = 0;
        }
    }
}

void PagePlay::onAppendToHistory()
{
    /// copy orig channel info to history
    QList<QStandardItem*> items = ChannelListModel::self()->findItems( tr( "My History" ), Qt::MatchFixedString | Qt::MatchCaseSensitive );
    QStandardItem* groupitem;
    if ( items.count() == 0 ) {
        QStandardItem* rootitem = ChannelListModel::self()->invisibleRootItem();
        groupitem = new QStandardItem( tr( "My History" ) );
        rootitem->appendRow( groupitem );
    }
    else {
        groupitem = items.first();
    }
    AbstractChannel* ch;
    int chtype = UNDEFINED;
    if ( ( channel->m_sourcetype & SOPCAST ) == SOPCAST ) {
        ch = new ChannelSopcast;
        chtype |= SOPCAST;
    }
    else if ( ( channel->m_sourcetype & PPLIVE ) == PPLIVE ) {
        ch = new ChannelPPLive;
        chtype |= PPLIVE;
    }
    else if ( ( channel->m_sourcetype & PPSTREAM ) == PPSTREAM ) {
        ch = new ChannelPPStream;
        chtype |= PPSTREAM;
    }
    else
        return;/// should not come here...  we do not support this imaginary type  ;)
    ch->m_name = channel->m_name;
    ch->m_url = channel->m_url;
    ch->m_kbps = channel->m_kbps;
    /// append history tag and remove possible bookmark tag
    ch->m_sourcetype = (source_t)( ( chtype | HISTORY ) & ~BOOKMARK );
    /// add to model
    QStandardItem* chitem = new QStandardItem( ch->m_name );
    chitem->setData( QVariant::fromValue( static_cast<QObject*>(ch) ) );
    groupitem->appendRow( chitem );
    /// append to history
    ChannelListHistory::self()->addChannel( ch );
}

#include "pageplay.moc"
