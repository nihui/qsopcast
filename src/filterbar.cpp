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

#include <QHBoxLayout>
#include <QCheckBox>

#include "abstractchannel.h"

#include "filterbar.h"

FilterBar* FilterBar::m_self = 0;

FilterBar* FilterBar::self()
{
    if ( !m_self )
        m_self = new FilterBar;
    return m_self;
}

FilterBar::FilterBar( QWidget* parent ) :
    QWidget( parent )
{
    QHBoxLayout* layout = new QHBoxLayout;
    // shrink the height and width
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 4 );
    setLayout( layout );

    sopcast = new QCheckBox( tr( "sopcast" ) );
    sopcast->setChecked( true );
    pplive = new QCheckBox( tr( "pplive" ) );
    pplive->setChecked( true );
    ppstream = new QCheckBox( tr( "ppstream" ) );
    ppstream->setChecked( true );
    bookmark = new QCheckBox( tr( "bookmark" ) );
    bookmark->setChecked( true );
    history = new QCheckBox( tr( "history" ) );
    history->setChecked( true );
    layout->addWidget( sopcast );
    layout->addWidget( pplive );
    layout->addWidget( ppstream );
    layout->addWidget( bookmark );
    layout->addWidget( history );
    connect( sopcast, SIGNAL( toggled( bool ) ), this, SLOT( updateFilter() ) );
    connect( pplive, SIGNAL( toggled( bool ) ), this, SLOT( updateFilter() ) );
    connect( ppstream, SIGNAL( toggled( bool ) ), this, SLOT( updateFilter() ) );
    connect( bookmark, SIGNAL( toggled( bool ) ), this, SLOT( updateFilter() ) );
    connect( history, SIGNAL( toggled( bool ) ), this, SLOT( updateFilter() ) );
}

void FilterBar::updateFilter()
{
    int filter = UNDEFINED;
    if ( sopcast->isChecked() )
        filter |= SOPCAST;
    if ( pplive->isChecked() )
        filter |= PPLIVE;
    if ( ppstream->isChecked() )
        filter |= PPSTREAM;
    if ( bookmark->isChecked() )
        filter |= BOOKMARK;
    if ( history->isChecked() )
        filter |= HISTORY;
    emit filterChanged( filter );
}

#include "filterbar.moc"
