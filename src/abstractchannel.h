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

#ifndef ABSTRACTCHANNEL_H
#define ABSTRACTCHANNEL_H

#include <QObject>
#include <QHash>
#include <QString>

typedef enum
{
    UNDEFINED   = 0x00000000,
    SOPCAST     = 0x00000001,
    PPLIVE      = 0x00000010,
    PPSTREAM    = 0x00000100,
    MMS         = 0x00001000,
    BOOKMARK    = 0x10000000,
    HISTORY     = 0x01000000
} source_t;

typedef enum { mp3, rmvb, wmv } format_t;

class AbstractChannel : public QObject
{
    Q_OBJECT
    public:
        AbstractChannel( QObject* parent = 0 );
        virtual ~AbstractChannel() {}
        virtual QString playerUrl() = 0;
        QString m_name;
        QString m_url;
        int m_kbps;
        format_t m_format;
        source_t m_sourcetype;
    signals:
        void channelStatusUpdated( const QString& msg );
    public slots:
        virtual void launch() = 0;
        virtual void play() = 0;
        virtual void stop() = 0;
        virtual void pause() = 0;
};

class AbstractChannelList : public QObject
{
    Q_OBJECT
    public:
        AbstractChannelList( QObject* parent = 0 );
        virtual ~AbstractChannelList() {}
    signals:
        void channellistRetrieveStatusUpdated( const QString& msg );
        void channellistRetrieved( const QHash< QString, QList<AbstractChannel*> >& data );
        void channellistRetrieved( const QHash< QString, QHash< QString, QList<AbstractChannel*> > >& data );
    public slots:
        virtual void retrieveChannels() = 0;
    private:
        int m_maxkbps;
};

#endif // ABSTRACTCHANNEL_H
