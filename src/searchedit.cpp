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

#include <QLineEdit>
#include "searchedit.h"

SearchEdit::SearchEdit( QWidget* parent ) :
    QLineEdit( parent )
{
    no_text = true;
    setText( tr( "Type here to search..." ) );
    connect( this, SIGNAL( textEdited( const QString& ) ),
             this, SLOT( onTextEdited( const QString& ) ) );
}

void SearchEdit::focusInEvent( QFocusEvent* e )
{
    if ( no_text )
        clear();
    QLineEdit::focusInEvent( e );
}

void SearchEdit::focusOutEvent( QFocusEvent* e )
{
    if ( text().isEmpty() ) {
        no_text = true;
        setText( tr( "Type here to search..." ) );
    }
    else
        no_text = false;
    QLineEdit::focusOutEvent( e );
}

 void SearchEdit::onTextEdited( const QString& text )
 {
     if ( !text.isEmpty() )
         no_text = false;
 }

#include "searchedit.moc"
