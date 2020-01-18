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

#ifndef CHANNELLISTMODEL_H
#define CHANNELLISTMODEL_H

#include <QStandardItemModel>

class MyStatusBar;

class AbstractChannel;

class ChannelListSopcast;
class ChannelListPPLive;
class ChannelListPPStream;
class ChannelListBookmark;
class ChannelListHistory;
class ChannelListArgument;

class ChannelListModel : public QStandardItemModel
{
    Q_OBJECT
    public:
        static ChannelListModel* self();
        ~ChannelListModel();

    signals:
        void channelListRefreshed() const;
    public slots:
        void refreshChannelList();
    private:
        MyStatusBar* statusbar;
        static ChannelListModel* m_self;
        ChannelListModel( QObject* parent = 0 );

        ChannelListSopcast* sop;
        ChannelListPPLive* pplive;
        ChannelListPPStream* ppstream;
        ChannelListBookmark* bookmark;
        ChannelListHistory* history;
        ChannelListArgument* argument;
    private slots:
        /// without sub groups
        void addChannelList( const QHash< QString, QList<AbstractChannel*> >& data );
        /// with sub groups
        void addChannelList( const QHash< QString, QHash< QString, QList<AbstractChannel*> > >& data );
        /// count the lists finished
        void countFinishedLists() const ;
};

#endif
