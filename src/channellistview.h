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

#ifndef CHANNELLISTVIEW_H
#define CHANNELLISTVIEW_H

#include <QTreeView>
#include <QString>

class QAction;
class QContextMenuEvent;
class QPainter;

class ChannelListView : public QTreeView
{
    Q_OBJECT
    public:
        static ChannelListView* self();
        ~ChannelListView();
    public slots:
        void onSearchTextChanged( const QString& searchkey );
        void onFilterChanged( int filter );
    protected:
        void contextMenuEvent( QContextMenuEvent* event );
    private:
        QAction* addToBookmark;
        QAction* removeBookmark;
        QAction* removeHistoryEntry;

        int m_filter;
        QString m_searchkey;
        void filterTree();

        static ChannelListView* m_self;
        ChannelListView( QWidget* parent = 0 );
    private slots:
        void onAddToBookmark();
        void onRemoveBookmark();
        void onRemoveHistoryEntry();
};

#endif // CHANNELLISTVIEW_H
