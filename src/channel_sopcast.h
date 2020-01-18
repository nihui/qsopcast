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

#ifndef CH_SOPCAST_H
#define CH_SOPCAST_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QString>

#include "abstractchannel.h"

class QProcess;
class QSocketNotifier;
class QTimer;

class ChannelSopcast : public AbstractChannel
{
    Q_OBJECT
    public:
        ChannelSopcast( QObject* parent = 0 );
        virtual ~ChannelSopcast();
        virtual QString playerUrl();

        int m_status;
        QString m_region;
        int m_class;
        int m_usercount;
        int m_sn;
        int m_visitcount;
        QString m_startfrom;
        int m_qs;
        int m_qc;
        QString m_description;

    public slots:
        virtual void launch();
        virtual void play() {}
        virtual void stop();
        virtual void pause() {}
    private:
        QProcess* sop;
        QSocketNotifier* snstatus;
        QTimer* echotimer;
        int outport;
        int inport;
    private slots:
        void parseStdout();
        void streamReady( int fd );
        void echo();
};

class ChannelListSopcast : public AbstractChannelList
{
    Q_OBJECT
    public:
        ChannelListSopcast( QObject* parent = 0 );
    public slots:
        virtual void retrieveChannels();
    private:
        QProcess* wget;
        QHash< QString, QList<AbstractChannel*> > channels;
    private slots:
        void parse();
};

#endif // CH_SOPCAST_H
