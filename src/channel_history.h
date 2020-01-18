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

#ifndef CH_HISTORY_H
#define CH_HISTORY_H

#include <QHash>
#include <QList>
#include <QString>

#include "abstractchannel.h"

class QObject;

class ChannelListHistory : public AbstractChannelList
{
    Q_OBJECT
    public:
        static ChannelListHistory* self();
        virtual ~ChannelListHistory();
    public slots:
        virtual void retrieveChannels();
        void addChannel( AbstractChannel* channel );
        void removeChannel( const QString& name );
    private:
        QList<AbstractChannel*> channels;
        void saveChannels();

        static ChannelListHistory* m_self;
        ChannelListHistory( QObject* parent = 0 );
};

#endif // CH_HISTORY_H
