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

#ifndef CH_PPSTREAM_H
#define CH_PPSTREAM_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QQueue>
#include <QString>

#include "abstractchannel.h"

class QBuffer;
class QProcess;

class ChannelPPStream : public AbstractChannel
{
    Q_OBJECT
    public:
        ChannelPPStream( QObject* parent = 0 );
        virtual ~ChannelPPStream();
        virtual QString playerUrl();

        int m_peercount;
        //         int m_sn;
        int m_quality;
        //         QString m_startfrom;
        //         QString m_description;

    public slots:
        virtual void launch();
        virtual void play() {}
        virtual void stop();
        virtual void pause() {}
    private:
        QProcess* ppstream;
        int outport;
    private slots:
        void parseStdout();
};

class QSignalMapper;

class ChannelListPPStream : public AbstractChannelList
{
    Q_OBJECT
    public:
        ChannelListPPStream( QObject* parent = 0 );
        virtual ~ChannelListPPStream();
    public slots:
        virtual void retrieveChannels();
    private:
        QProcess* wget;
        QHash< QString, QList<AbstractChannel*> > channels;
        QHash< QString, QHash< QString, QList<AbstractChannel*> > > subchannels;

        QSignalMapper* signalMapper;
        QSignalMapper* signalMapper2;
        QHash<int, QString> groupnamehash;
        QHash<int, QString> subgroupnamehash;

        typedef struct subgentry_t { int groupid; int grouptype; int contentnum; } subgentry_t;
        QQueue< subgentry_t > subgroupqueue;
        typedef struct chentry_t { int groupid; int subgroupid; int grouptype; int contentnum; } chentry_t;
        QQueue< chentry_t > channelqueue;

        void clearChannels();
        void clearSubChannels();

        void retrieveSubGroup( int groupid, int grouptype, int contentnum );
        void retrieveChannel( int groupid, int subgroupid, int grouptype, int contentnum );
        
        void retrieveSubGroupsSync();
        void retrieveChannelsSync();

        int channelFilesCount;
        void collectChannelFiles();

    private slots:
        void parseGroup();
        void parseSubGroup( const QString& localcacheurl );
        void parseChannel( const QString& localcacheurl );
};

#endif // CH_PPSTREAM_H
