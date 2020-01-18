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

#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QStandardItem>
#include <QPainter>

#include "abstractchannel.h"
#include "channel_bookmark.h"
#include "channel_history.h"
#include "channel_sopcast.h"
#include "channel_pplive.h"
#include "channel_ppstream.h"
#include "channellistmodel.h"

#include "channellistview.h"

ChannelListView* ChannelListView::m_self = 0;

ChannelListView* ChannelListView::self()
{
    if ( !m_self )
        m_self = new ChannelListView;
    return m_self;
}

ChannelListView::~ChannelListView()
{
    m_self = 0;
}

ChannelListView::ChannelListView( QWidget* parent ) :
    QTreeView( parent )
{
    setEditTriggers( QAbstractItemView::NoEditTriggers );

    addToBookmark = new QAction( tr( "Add to bookmark" ), this );
    connect( addToBookmark, SIGNAL( triggered( bool ) ), this, SLOT( onAddToBookmark() ) );
    removeBookmark = new QAction( tr( "Remove bookmark" ), this );
    connect( removeBookmark, SIGNAL( triggered( bool ) ), this, SLOT( onRemoveBookmark() ) );
    removeHistoryEntry = new QAction( tr( "Remove this history entry" ), this );
    connect( removeHistoryEntry, SIGNAL( triggered( bool ) ), this, SLOT( onRemoveHistoryEntry() ) );

    m_filter = 0x11111111;/// all types
}

void ChannelListView::contextMenuEvent( QContextMenuEvent* event )
{
    QModelIndex index = currentIndex();
    // check if it is a null channel or a group entry
    if ( index.isValid() && !model()->hasChildren( index ) ) {
        QStandardItem* item = static_cast<ChannelListModel*>(model())->itemFromIndex( index );
        AbstractChannel* channel = static_cast<AbstractChannel*>(item->data().value<QObject*>());

        QMenu menu( this );
        source_t sourcetype = channel->m_sourcetype;
        if ( ( sourcetype & BOOKMARK ) != BOOKMARK )
            menu.addAction( addToBookmark );
        else
            menu.addAction( removeBookmark );
        if ( ( sourcetype & HISTORY ) == HISTORY )
            menu.addAction( removeHistoryEntry );
        menu.exec( event->globalPos() );
    }
}

void ChannelListView::onAddToBookmark()
{
    QStandardItemModel* listmodel = ChannelListModel::self();
    QModelIndex index = currentIndex();
    QStandardItem* currentitem = listmodel->itemFromIndex( index );
    AbstractChannel* channel = static_cast<AbstractChannel*>(currentitem->data().value<QObject*>());

    if ( listmodel->findItems( channel->m_name, Qt::MatchFixedString | Qt::MatchCaseSensitive ).count() > 0 ) {
        /// this channel is already in the bookmark
        return;
    }

    QList<QStandardItem*> items = listmodel->findItems( tr( "My Bookmarks" ), Qt::MatchFixedString | Qt::MatchCaseSensitive );
    QStandardItem* groupitem;
    if ( items.count() == 0 ) {
        QStandardItem* rootitem = listmodel->invisibleRootItem();
        groupitem = new QStandardItem( tr( "My Bookmarks" ) );
        rootitem->appendRow( groupitem );
    }
    else {
        groupitem = items.first();
    }
    /// copy orig channel info to bookmark
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
    /// append bookmark tag and remove poosible history tag
    ch->m_sourcetype = (source_t)( ( chtype | BOOKMARK ) & ~HISTORY );
    /// add to model
    QStandardItem* chitem = new QStandardItem( ch->m_name );
    chitem->setData( QVariant::fromValue( static_cast<QObject*>(ch) ) );
    groupitem->appendRow( chitem );
    /// add to bookmark
    ChannelListBookmark::self()->addChannel( ch );
}

void ChannelListView::onRemoveBookmark()
{
    QStandardItemModel* listmodel = ChannelListModel::self();
    QModelIndex index = currentIndex();
    QStandardItem* currentitem = listmodel->itemFromIndex( index );
    AbstractChannel* channel = static_cast<AbstractChannel*>(currentitem->data().value<QObject*>());
    /// remove from bookmark first, otherwise we can not get the channel point anyway
    ChannelListBookmark::self()->removeChannel( channel->m_name );
    /// remove from model
    QList<QStandardItem*> items = listmodel->findItems( tr( "My Bookmarks" ), Qt::MatchFixedString | Qt::MatchCaseSensitive );
    QStandardItem* groupitem = items.first();
    groupitem->removeRow( currentitem->row() );
    /// remove the group if it has no children
    if ( groupitem->rowCount() == 0 ) {
        QStandardItem* rootitem = ChannelListModel::self()->invisibleRootItem();
        rootitem->removeRow( groupitem->row() );
    }
}

void ChannelListView::onRemoveHistoryEntry()
{
    QStandardItemModel* listmodel = ChannelListModel::self();
    QModelIndex index = currentIndex();
    QStandardItem* currentitem = listmodel->itemFromIndex( index );
    AbstractChannel* channel = static_cast<AbstractChannel*>(currentitem->data().value<QObject*>());
    /// remove from history first, otherwise we can not get the channel point anyway
    ChannelListHistory::self()->removeChannel( channel->m_name );
    /// remove from model
    QList<QStandardItem*> items = listmodel->findItems( tr( "My History" ), Qt::MatchFixedString | Qt::MatchCaseSensitive );
    QStandardItem* groupitem = items.first();
    groupitem->removeRow( currentitem->row() );
    /// remove the group if it has no children
    if ( groupitem->rowCount() == 0 ) {
        QStandardItem* rootitem = ChannelListModel::self()->invisibleRootItem();
        rootitem->removeRow( groupitem->row() );
    }
}

void ChannelListView::onSearchTextChanged( const QString& searchkey )
{
    m_searchkey = searchkey;
    filterTree();

    /// expand for usability
    if ( m_searchkey.isEmpty() )
        collapseAll();
    else
        expandAll();
}

void ChannelListView::onFilterChanged( int filter )
{
    m_filter = filter;
    filterTree();
}

void ChannelListView::filterTree()
{
    QStandardItem* rootitem = ChannelListModel::self()->invisibleRootItem();
    if ( rootitem->hasChildren() ) {
        int groupcount = rootitem->rowCount();
        int i = 0;
        for ( ; i<groupcount; ++i ) {
            QStandardItem* groupitem = rootitem->child( i );
            if ( groupitem->hasChildren() ) {
                int subcount = groupitem->rowCount();
                int j = 0;
                int hiddensubcount = 0;
                for ( ; j<subcount; ++j ) {
                    QStandardItem* subitem = groupitem->child( j );
                    if ( subitem->hasChildren() ) {
                        int chcount = subitem->rowCount();
                        int k = 0;
                        int hiddenchcount = 0;
                        for ( ; k<chcount; ++k ) {
                            QStandardItem* chitem = subitem->child( k );
                            /// hide groups -> subgroups -> channels
                            AbstractChannel* channel = static_cast<AbstractChannel*>(chitem->data().value<QObject*>());
                            if ( ( channel->m_sourcetype & m_filter ) != UNDEFINED
                                && chitem->text().contains( m_searchkey, Qt::CaseInsensitive ) )
                                setRowHidden( k, subitem->index(), false );
                            else {
                                setRowHidden( k, subitem->index(), true );
                                ++hiddenchcount;
                            }
                        }
                        /// hide groups -> subgroups
                        if ( hiddenchcount == chcount ) {
                            setRowHidden( j, groupitem->index(), true );
                            ++hiddensubcount;
                        }
                        else
                            setRowHidden( j, groupitem->index(), false );
                    }
                    else {
                        /// hide groups -> channels
                        AbstractChannel* channel = static_cast<AbstractChannel*>(subitem->data().value<QObject*>());
                        if ( ( channel->m_sourcetype & m_filter ) != UNDEFINED
                            && subitem->text().contains( m_searchkey, Qt::CaseInsensitive ))
                            setRowHidden( j, groupitem->index(), false );
                        else {
                            setRowHidden( j, groupitem->index(), true );
                            ++hiddensubcount;
                        }
                    }
                }
                /// hide groups
                if ( hiddensubcount == subcount )
                    setRowHidden( i, rootitem->index(), true );
                else
                    setRowHidden( i, rootitem->index(), false );
            }
        }
    }
}

#include "channellistview.moc"
