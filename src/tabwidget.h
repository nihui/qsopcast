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

#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>

// #include <QMouseEvent>

class ChannelListModel;
class ChannelListView;

class TabWidget : public QTabWidget
{
    Q_OBJECT
    public:
        static TabWidget* self();
        ~TabWidget();
    private:
        ChannelListModel* model;
        ChannelListView* listview;

        static TabWidget* m_self;
        TabWidget( QWidget* parent = 0 );

    private slots:
        void insertPlayTab();
        void removeCurrentTab();
        void insertRecordTab();
};

#endif // TABWIDGET_H
