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

#include <QMouseEvent>
#include <QWidget>
#include <QPushButton>
#include <QModelIndex>
#include <QStandardItem>

#include "channellistmodel.h"
#include "channellistview.h"
#include "pageplay.h"
#include "pagerecord.h"

#include "tabwidget.h"

TabWidget* TabWidget::m_self = 0;

TabWidget* TabWidget::self()
{
    if ( !m_self )
        m_self = new TabWidget;
    return m_self;
}

TabWidget::TabWidget( QWidget* parent ) :
        QTabWidget( parent )
{
    ///topleft corner widget
    QPushButton* newpage = new QPushButton( tr( "New Page" ) );
    setCornerWidget( newpage, Qt::TopLeftCorner );
    connect( newpage, SIGNAL( clicked() ), this, SLOT( insertPlayTab() ) );
    ///topright corner widget
    QPushButton* record = new QPushButton( tr( "Record" ) );
    setCornerWidget( record, Qt::TopRightCorner );
    connect( record, SIGNAL( clicked() ), this, SLOT( insertRecordTab() ) );
    
    ///tab play
    insertPlayTab();

    listview = ChannelListView::self();
    model = ChannelListModel::self();
}

TabWidget::~TabWidget()
{
    m_self = 0;
}

void TabWidget::insertPlayTab()
{
    PagePlay* pageplay = new PagePlay;
    connect( pageplay->buttonclose, SIGNAL( clicked() ), this, SLOT( removeCurrentTab() ) );
    insertTab( currentIndex() + 1, pageplay, tr( "play" ) );
    setCurrentWidget( pageplay );
}

void TabWidget::removeCurrentTab()
{
    if ( count() > 1 ) {
        QWidget* tab = currentWidget();
        removeTab( currentIndex() );
        delete tab;
    }
}

void TabWidget::insertRecordTab()
{
    PageRecord* pagerecord = new PageRecord;
    connect( pagerecord->buttonclose, SIGNAL( clicked() ), this, SLOT( removeCurrentTab() ) );
    insertTab( currentIndex() + 1, pagerecord, tr( "record" ) );
    setCurrentWidget( pagerecord );
}

#include "tabwidget.moc"
