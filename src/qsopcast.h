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

#ifndef QSOPCAST_H
#define QSOPCAST_H

#include <QMainWindow>
#include <QSystemTrayIcon>

class QToolButton;
class QLineEdit;
class QSlider;
class QCloseEvent;
class QMenu;
class QKeyEvent;

class ChannelListModel;
class ChannelListView;
class ChannelListBookmark;
class ChannelListHistory;
class ChannelListArgument;
class MenuBar;
class SearchEdit;
class FilterBar;
class TabWidget;
class MyStatusBar;

class QSopcast: public QMainWindow
{
    Q_OBJECT
    public:
        QSopcast( QWidget * parent = 0 );
        ~QSopcast();

    private:
        QToolButton* buttonchannel;
        QToolButton* buttonexpand;
        ChannelListModel* model;
        ChannelListView* channellist;
        MenuBar* menubar;
        FilterBar* filterbar;
        TabWidget* tabwidget;
        MyStatusBar* statusbar;
        SearchEdit* editsearch;

        ChannelListBookmark* bookmark;
        ChannelListHistory* history;
        ChannelListArgument* argument;

        QSystemTrayIcon* trayIcon;
        QMenu* trayIconMenu;

    protected:
        void keyPressEvent( QKeyEvent* e );

    private:
        void setupTrayIcon();

    private slots:
        void onButtonExpandClicked();
        void onRefreshListFinished();
        void closeEvent( QCloseEvent* event );
        void iconActivated( QSystemTrayIcon::ActivationReason reason );
};

#endif // QSOPCAST_H
