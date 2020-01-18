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

#include <QApplication>
#include <QSettings>

#include "config.h"

#include "menubar.h"

MenuBar::MenuBar( QWidget* parent ):
        QMenuBar( parent )
{
    ///initialization
    QSettings settings;

    ///menu file
    menu_file = addMenu( tr( "&File" ) );
    QAction* exitAct = new QAction( tr( "&Quit" ), this );
    exitAct->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_Q ) );
    connect( exitAct, SIGNAL( triggered() ), qApp, SLOT( quit() ) );
    menu_file->addAction( exitAct );

    ///menu config
    menu_config = addMenu( tr( "&Setting" ) );
//     menu_config->setCheckable( true );

    ///channel color
//     submenu_color = menu_config->addMenu( tr( "Channel Color" ) );
//     submenu_color->setCheckable( true );
//     submenu_color->addAction( tr( "User" ) );
//     submenu_color->addAction( tr( "Visit" ) );
//     submenu_color->addAction( tr( "Qs" ) );
//     submenu_color->addAction( tr( "Qc" ) );
//     submenu_color->addAction( tr( "Kbps" ) );

//     color_column = settings.readNumEntry( "/qsopcast/ChannelColor", USER_COLUMN );//FIXME:port

//     submenu_color->setItemChecked( color_column, true );//FIXME:port
//     connect( submenu_color, SIGNAL( activated( int ) ), this, SLOT( onSwitchColor( int ) ) );
//     menu_config->insertItem( tr( "Channel Color" ), submenu_color,
//                              CHANNEL_COLOR );
    //enable auto restart player
//     QAction* autorestartplayerAct = new QAction( tr( "&Enable Auto Restarting Player" ), this );
//     autorestartplayerAct->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_P ) );
//     connect( autorestartplayerAct, SIGNAL( triggered() ), this, SLOT( onAutoRestartPlayerToggled() ) );
//     menu_config->addAction( autorestartplayerAct );
//     menu_config->insertItem( tr( "&Enable Auto Restarting Player" ), this,
//                              SLOT( onAutoRestartPlayerToggled() ),
//                              Qt::CTRL + Qt::Key_P, AUTO_RESTART_PLAYER );
//     menu_config->setItemChecked( AUTO_RESTART_PLAYER,
//                                  settings.
//                                  readBoolEntry
//                                  ( "/qsopcast/enableautorestartplayer", FALSE ) );//FIXME: port
    ///configuration
    QAction* configAct = new QAction( tr( "&Configure..." ), this );
    configAct->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_F ) );
    connect( configAct, SIGNAL( triggered() ), this, SLOT( onconfigclicked() ) );
    menu_config->addAction( configAct );

    ///menu help
    menu_help = addMenu( tr( "&Help" ) );
    QAction* aboutAct = new QAction( tr( "&About..." ), this );
    aboutAct->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_H ) );
    connect( aboutAct, SIGNAL( triggered() ), this, SLOT( about() ) );
    menu_help->addAction( aboutAct );
}


MenuBar::~MenuBar()
{
//     QSettings settings;
//     settings.setValue( "/qsopcast/ChannelColor", color_column ); ,//FIXME: port
//     settings.setValue( "/qsopcast/enableautorestartplayer",
//                          menu_config->isItemChecked( AUTO_RESTART_PLAYER ) );
}

// void
// MenuBar::onSwitchColor( int id )
// {/*
//     mainwindow->channel->switchChannelColor( id );
//     color_column = id;
//     for ( uint index = 0; index < submenu_color->count(); index++ )
//         submenu_color->setItemChecked( submenu_color->idAt( index ), false );
//     submenu_color->setItemChecked( id, true );*/
// }

// void
// MenuBar::onAutoRestartPlayerToggled()
// {/*
//     menu_config->setItemChecked( AUTO_RESTART_PLAYER,
//                                  !menu_config->
//                                  isItemChecked( AUTO_RESTART_PLAYER ) );*/
// }

void MenuBar::onconfigclicked()
{
    Config cfgdlg( this );
    cfgdlg.exec();
}

#include <QMessageBox>
#include <QPixmap>
#include <QImage>
#include "version.h"
void MenuBar::about()
{
    QMessageBox m;
    m.setWindowTitle( QString( tr( "About qsopcast %1" ) ).arg( QString( qsopcast_version ) ) );
    m.setIconPixmap( QPixmap::fromImage( QImage( "/usr/share/pixmaps/mozart.png" ) ) );//FIXME use relative path!!
    m.setText( QString( tr( "qsopcast: A QT front-end to p2p TV.\n"
                   "version: %1\n"
                   "Copyright (C) 2005, 2006, 2009 Liu Di\n"
                   "Copyright (C) 2007 Wei Lian\n"
                   "Copyright (C) 2009, 2010 Ni Hui\n"
                   "http://code.google.com/p/qsopcast/" ) ).arg( QString( qsopcast_version ) ) );
    m.exec();
}

#include "menubar.moc"
