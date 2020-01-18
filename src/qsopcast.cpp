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
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QLineEdit>
#include <QKeyEvent>
#include <QSettings>
#include <QAction>
#include <QMenu>
#include <QCloseEvent>
#include <QSystemTrayIcon>

#include "channellistmodel.h"
#include "channellistview.h"
#include "channel_bookmark.h"
#include "channel_history.h"
#include "channel_argument.h"
#include "menubar.h"
#include "searchedit.h"
#include "filterbar.h"
#include "tabwidget.h"
#include "mystatusbar.h"

#include "qsopcast.h"

QSopcast::QSopcast( QWidget* parent ):
        QMainWindow( parent )
{
    ///menubar
    menubar = new MenuBar;
    setMenuBar( menubar );

    QWidget* mainwidget = new QWidget;
    setCentralWidget( mainwidget );
    QVBoxLayout* mainlayout = new QVBoxLayout( mainwidget );
    mainlayout->setContentsMargins( 11, 2, 11, 0 );
    mainlayout->setSpacing( 2 );

    ///channel row
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setContentsMargins( 0, 0, 0, 0 );
    hbox->setSpacing( 2 );
    mainlayout->addLayout( hbox );
    ///button channel
    buttonchannel = new QToolButton;
    buttonchannel->setAutoRaise( true );
    buttonchannel->setCheckable( true );
    buttonchannel->setText( tr( "Channel" ) );
    hbox->addWidget( buttonchannel );
    ///button expand
    buttonexpand = new QToolButton;
    buttonexpand->setText( tr( "Expand" ) );
    buttonexpand->setAutoRaise( true );
    hbox->addWidget( buttonexpand );
    connect( buttonexpand, SIGNAL( clicked() ), this, SLOT( onButtonExpandClicked() ) );
    ///edit search
    editsearch = new SearchEdit;
    hbox->addWidget( editsearch );

    /// filter bar
    filterbar = FilterBar::self();
    mainlayout->addWidget( filterbar );

    /// channel list
    model = ChannelListModel::self();
    channellist = ChannelListView::self();

    channellist->setModel( model );
    channellist->setSortingEnabled( true );
    mainlayout->addWidget( channellist, 1 );

    connect( buttonchannel, SIGNAL( pressed() ), model, SLOT( refreshChannelList() ) );
    connect( model, SIGNAL( channelListRefreshed() ), this, SLOT( onRefreshListFinished() ) );
    connect( buttonchannel, SIGNAL( toggled( bool ) ), buttonchannel, SLOT( setDisabled( bool ) ) );
    connect( editsearch, SIGNAL( textEdited( const QString& ) ), channellist, SLOT( onSearchTextChanged( const QString& ) ) );
    connect( filterbar, SIGNAL( filterChanged( int ) ), channellist, SLOT( onFilterChanged( int ) ) );
    // retrieve list at app start
    buttonchannel->click();

    /// tab widget
    tabwidget = TabWidget::self();
    mainlayout->addWidget( tabwidget, 0 );

    /// status bar
    statusbar = MyStatusBar::self();
    setStatusBar( statusbar );

    /// tray icon
    setupTrayIcon();
#if 0
    QSettings settings;
    QString sopcastbin = settings.value( "/qsopcast/sopcastbin", "/usr/bin/sp-sc-auth" ).toString();
    if ( QFile( sopcastbin ).exists() && QFileInfo( sopcastbin ).isExecutable() ) {
        return;
    }
    else {
        switch ( QMessageBox::warning( this, tr( "Cannot Find sopcast" ),
                                       tr( "Could not find sopcast execute program,\n"
                                           "Would you config it now?\n" ),
                                       tr( "&Config" ), tr( "Ca&ncel" ), 0, 2
                                     ) ) {
            case 0:
                vboxconfig->
                move( mapToGlobal
                      ( QPoint
                        (( width() - vboxconfig->width() ) / 2,
                         ( height() -
                           vboxconfig->height() ) / 2 ) ) );
                vboxconfig->show();
            case 2:
                return;
        }
    }
#endif
    bookmark = ChannelListBookmark::self();
    history = ChannelListHistory::self();
    argument = ChannelListArgument::self();

    /// restore width and height
    QSettings settings;
    int w = settings.value( "/qsopcast/width", "444" ).toInt();
    int h = settings.value( "/qsopcast/height", "524" ).toInt();
    setWindowIcon( QIcon( "/usr/share/pixmaps/sopcast.xpm" ) );
    resize( w, h );
    editsearch->setFocus();
}

QSopcast::~QSopcast()
{
    delete model;
    delete bookmark;
    delete history;
    delete argument;
    ///save width and height
    if ( !isMaximized() ) {
        QSettings settings;
        settings.setValue( "/qsopcast/width", size().width() );
        settings.setValue( "/qsopcast/height", size().height() );
    }
}

void QSopcast::onButtonExpandClicked()
{
    static bool isexpanded = false;
    if ( isexpanded ) {
        channellist->collapseAll();
        isexpanded = false;
        buttonexpand->setText( tr( "Expand" ) );
    }
    else {
        channellist->expandAll();
        isexpanded = true;
        buttonexpand->setText( tr( "Collapse" ) );
    }
}

void QSopcast::onRefreshListFinished()
{
    buttonchannel->setChecked( false );
    buttonchannel->setDisabled( false );
}

void QSopcast::setupTrayIcon()
{
    QAction* minimizeAction = new QAction( tr( "Mi&nimize" ), this );
    connect( minimizeAction, SIGNAL( triggered() ), this, SLOT( hide() ) );
    QAction* maximizeAction = new QAction( tr( "Ma&ximize" ), this );
    connect( maximizeAction, SIGNAL( triggered() ), this, SLOT( showMaximized() ) );
    QAction* restoreAction = new QAction( tr( "&Restore" ), this );
    connect( restoreAction, SIGNAL( triggered() ), this, SLOT( showNormal() ) );
    QAction* quitAction = new QAction( tr( "&Quit" ), this );
    connect( quitAction, SIGNAL( triggered() ), qApp, SLOT( quit() ) );

    trayIconMenu = new QMenu( this );
    trayIconMenu->addAction( minimizeAction );
    trayIconMenu->addAction( maximizeAction );
    trayIconMenu->addAction( restoreAction );
    trayIconMenu->addSeparator();
    trayIconMenu->addAction( quitAction );

    trayIcon = new QSystemTrayIcon( this );
    trayIcon->setContextMenu( trayIconMenu );
    connect( trayIcon, SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),
             this, SLOT( iconActivated( QSystemTrayIcon::ActivationReason ) ) );

    trayIcon->setIcon( QIcon( "/usr/share/pixmaps/sopcast.xpm" ) );
    trayIcon->show();
}

void QSopcast::closeEvent( QCloseEvent* event )
{
    hide();
    event->ignore();
}

void QSopcast::iconActivated( QSystemTrayIcon::ActivationReason reason )
{
    switch ( reason ) {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
            setVisible( !isVisible() );
            break;
        default:
            break;
    }
}

void QSopcast::keyPressEvent( QKeyEvent* e )
{
        editsearch->setFocus();
        QMainWindow::keyPressEvent( e );
}

#include "qsopcast.moc"
