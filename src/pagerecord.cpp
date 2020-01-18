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
#include "tabwidget.h"
#include "recorder.h"

#include "pagerecord.h"

PageRecord::PageRecord( QWidget* parent ):
        QWidget( parent )
{
    QHBoxLayout* layout = new QHBoxLayout;
    // shrink the height and width
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 2 );
    setLayout( layout );

    ///button record
    buttonrecord = new QPushButton( tr( "Record" ) );
    buttonrecord->setCheckable( true );
    connect( buttonrecord, SIGNAL( toggled( bool ) ), this, SLOT( onButtonRecordToggled( bool ) ) );
    layout->addWidget( buttonrecord );
    ///button delete
    buttonclose = new QPushButton( tr( "x" ) );
    layout->addWidget( buttonclose );

    channel = 0;
    recorder = 0;
}

PageRecord::~PageRecord()
{
    if ( channel ) {
        channel->stop();
        channel = 0;
    }
    if ( recorder ) {
        recorder->stopRecorder();
        delete recorder;
        recorder = 0;
    }
}

void PageRecord::onButtonRecordToggled( bool t )
{
    if ( t ) {
        QModelIndex index = ChannelListView::self()->currentIndex();
        // check if it is a null channel or a group entry
        if ( index.isValid() && !ChannelListView::self()->model()->hasChildren( index ) ) {
            QStandardItem* item = ChannelListModel::self()->itemFromIndex( index );
            channel = static_cast<AbstractChannel*>( item->data().value<QObject*>() );
            if ( channel->m_sourcetype != SOPCAST ) {
                /// FIXME: NOTE: non sopcast channel is not support ATM
                channel = 0;
                buttonrecord->setChecked( false );
                return;
            }
            TabWidget::self()->setTabText( TabWidget::self()->currentIndex(), channel->m_name );
            channel->launch();

            recorder = new Recorder( this );
            recorder->setUrl( channel->playerUrl() );
            recorder->setChannelName( channel->m_name );
            recorder->startRecorder();
        }
    }
    else {
        if ( recorder ) {
            recorder->stopRecorder();
            delete recorder;
            recorder = 0;
        }
        TabWidget::self()->setTabText( TabWidget::self()->currentIndex(), tr( "record" ) );
        if ( channel ) {
            channel->stop();
            channel = 0;
        }
    }
}

#include "pagerecord.moc"
