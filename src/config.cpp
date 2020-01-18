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

#include <limits.h> // for INT_MAX

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QWidget>
#include <QSettings>
#include <QDir>

#include "config.h"

Config::Config( QWidget* parent ):
        QDialog( parent )
{
    setWindowTitle( tr( "qsopcast configuration" ) );
    /// gui
    QVBoxLayout* mainlayout = new QVBoxLayout;
    setLayout( mainlayout );

    QTabWidget* tabwidget = new QTabWidget;
    mainlayout->addWidget( tabwidget );

    /// general cfg tab
    QWidget* generalcfg = new QWidget;
    QFormLayout* generalcfglayout = new QFormLayout;
    player_mp3 = new QLineEdit;
    player_rmvb = new QLineEdit;
    player_wmv = new QLineEdit;
    recorddir = new QLineEdit;
    generalcfglayout->addRow( tr( "Player(mp3): " ), player_mp3 );
    generalcfglayout->addRow( tr( "Player(rmvb): " ), player_rmvb );
    generalcfglayout->addRow( tr( "Player(wmv): " ), player_wmv );
    generalcfglayout->addRow( tr( "Record directory: " ), recorddir );
    generalcfg->setLayout( generalcfglayout );
    tabwidget->addTab( generalcfg, tr( "&General" ) );

    /// sopcast cfg tab
    QWidget* sopcastcfg = new QWidget;
    QVBoxLayout* subsopcastlayout = new QVBoxLayout;
    QFormLayout* sopcastcfglayout = new QFormLayout;
    QFormLayout* sopcastadvancedcfglayout = new QFormLayout;
    sopcast_channelurl = new QLineEdit;
    sopcast_binurl = new QLineEdit;
    sopcast_forcetcp = new QCheckBox;
    sopcast_useexittime = new QCheckBox;
    sopcast_exittime = new QSpinBox;
    sopcast_exittime->setSuffix( tr( " seconds" ) );
    sopcast_exittime->setRange( 1, INT_MAX );
    sopcast_useauth = new QCheckBox;
    sopcast_username = new QLineEdit;
    sopcast_password = new QLineEdit;
    sopcastcfglayout->addRow( tr( "Channel URL: " ), sopcast_channelurl );
    sopcastcfglayout->addRow( tr( "Execute: " ), sopcast_binurl );
    sopcastadvancedcfglayout->addRow( tr( "Force use TCP: " ), sopcast_forcetcp );
    sopcastadvancedcfglayout->addRow( tr( "Enable auto-exit: " ), sopcast_useexittime );
    sopcastadvancedcfglayout->addRow( tr( "Exit channel after " ), sopcast_exittime );
    sopcastadvancedcfglayout->addRow( tr( "Use authentication: " ), sopcast_useauth );
    sopcastadvancedcfglayout->addRow( tr( "Username: " ), sopcast_username );
    sopcastadvancedcfglayout->addRow( tr( "Password: " ), sopcast_password );
    subsopcastlayout->addLayout( sopcastcfglayout );
    subsopcastlayout->addLayout( sopcastadvancedcfglayout );
    sopcastcfg->setLayout( subsopcastlayout );
    tabwidget->addTab( sopcastcfg, tr( "&Sopcast" ) );

    connect( sopcast_useexittime, SIGNAL( toggled( bool ) ), sopcast_exittime, SLOT( setEnabled( bool ) ) );
    connect( sopcast_useauth, SIGNAL( toggled( bool ) ), sopcast_username, SLOT( setEnabled( bool ) ) );
    connect( sopcast_useauth, SIGNAL( toggled( bool ) ), sopcast_password, SLOT( setEnabled( bool ) ) );

    /// pplive cfg tab
    QWidget* pplivecfg = new QWidget;
    QFormLayout* pplivecfglayout = new QFormLayout;
    pplive_channelurl = new QLineEdit;
    pplive_binurl = new QLineEdit;
    pplivecfglayout->addRow( tr( "Channel URL: " ), pplive_channelurl );
    pplivecfglayout->addRow( tr( "Execute: " ), pplive_binurl );
    pplivecfg->setLayout( pplivecfglayout );
    tabwidget->addTab( pplivecfg, tr( "PP&Live" ) );

    /// ppstream cfg tab
    QWidget* ppstreamcfg = new QWidget;
    QVBoxLayout* subppstreamlayout = new QVBoxLayout;
    QFormLayout* ppstreamcfglayout = new QFormLayout;
    QFormLayout* ppstreamadvancedcfglayout = new QFormLayout;
    ppstream_channelurl = new QLineEdit;
    ppstream_binurl = new QLineEdit;
    ppstream_clearcache = new QPushButton( tr( "&Clear playlist cache" ) );
    ppstreamcfglayout->addRow( tr( "Channel URL: " ), ppstream_channelurl );
    ppstreamcfglayout->addRow( tr( "Execute: " ), ppstream_binurl );
    ppstreamadvancedcfglayout->addRow( ppstream_clearcache );
    subppstreamlayout->addLayout( ppstreamcfglayout );
    subppstreamlayout->addLayout( ppstreamadvancedcfglayout );
    ppstreamcfg->setLayout( subppstreamlayout );
    tabwidget->addTab( ppstreamcfg, tr( "PPS&tream" ) );

    /// bookmark cfg
    QWidget* bookmarkcfg = new QWidget;
    QFormLayout* bookmarkcfglayout = new QFormLayout;
    bookmark_fileurl = new QLineEdit;
    bookmarkcfglayout->addRow( tr( "Bookmark file: " ), bookmark_fileurl );
    bookmarkcfg->setLayout( bookmarkcfglayout );
    tabwidget->addTab( bookmarkcfg, tr( "&Bookmark" ) );

    /// history cfg
    QWidget* historycfg = new QWidget;
    QFormLayout* historycfglayout = new QFormLayout;
    history_fileurl = new QLineEdit;
    history_maxcount = new QSpinBox;
    history_maxcount->setRange( 0, 100 );
    historycfglayout->addRow( tr( "History file: " ), history_fileurl );
    historycfglayout->addRow( tr( "Max record count: " ), history_maxcount );
    historycfg->setLayout( historycfglayout );
    tabwidget->addTab( historycfg, tr( "&History" ) );

    /// settings
    QSettings settings;
    player_mp3->setText( settings.value( "/qsopcast/player_mp3", "xmms" ).toString() );
    player_rmvb->setText( settings.value( "/qsopcast/player_rmvb", "mplayer" ).toString() );
    player_wmv->setText( settings.value( "/qsopcast/player_wmv", "mplayer" ).toString() );
    recorddir->setText( settings.value( "/qsopcast/recorddir", QDir::homePath() + "/SOPCAST/" ).toString() );
    sopcast_channelurl->setText( settings.value( "/qsopcast/sopcast_channelurl", "http://channel.sopcast.com/gchlxml" ).toString() );
    sopcast_binurl->setText( settings.value( "/qsopcast/sopcast_bin", "/usr/bin/sp-sc-auth" ).toString() );
    sopcast_forcetcp->setChecked( settings.value( "/qsopcast/sopcast_forcetcp", false ).toBool() );
    sopcast_useexittime->setChecked( settings.value( "/qsopcast/sopcast_useexittime", false ).toBool() );
    sopcast_exittime->setValue( settings.value( "/qsopcast/sopcast_exittime", "1000" ).toInt() );
    sopcast_exittime->setEnabled( sopcast_useexittime->isChecked() );
    sopcast_useauth->setChecked( settings.value( "/qsopcast/sopcast_useauth", false ).toBool() );
    sopcast_username->setText( settings.value( "/qsopcast/sopcast_username", "root" ).toString() );
    sopcast_password->setText( settings.value( "/qsopcast/sopcast_password", "root" ).toString() );
    sopcast_username->setEnabled( sopcast_useauth->isChecked() );
    sopcast_password->setEnabled( sopcast_useauth->isChecked() );
    pplive_channelurl->setText( settings.value( "/qsopcast/pplive_channelurl", "http://vnet.pplive.com/vnet2.xml" ).toString() );
    pplive_binurl->setText( settings.value( "/qsopcast/pplive_bin", "/usr/bin/xpplive" ).toString() );
    ppstream_channelurl->setText( settings.value( "/qsopcast/ppstream_channelurl", "http://playlist.pps.tv/classfortv.php" ).toString() );
    ppstream_binurl->setText( settings.value( "/qsopcast/ppstream_bin", "/usr/bin/xpps" ).toString() );

    bookmark_fileurl->setText( settings.value( "/qsopcast/bookmark_fileurl", QDir::homePath() + "/.qsopcast/bookmark.xml" ).toString() );
    history_fileurl->setText( settings.value( "/qsopcast/history_fileurl", QDir::homePath() + "/.qsopcast/history.xml" ).toString() );
    history_maxcount->setValue( settings.value( "/qsopcast/history_maxcount", "10" ).toInt() );
    #if 0
    label = new QLabel( tr( "sopcast URL header:" ), hbox );
    editchannelheader =
        new QLineEdit( settings.
                       readEntry( "/qsopcast/channelheader", "sop://" ), hbox );
    editchannelheader->setReadOnly( TRUE );
    #endif

    QHBoxLayout* buttonbox = new QHBoxLayout;
    QPushButton* buttoncancel = new QPushButton( tr( "&Cancel" ) );
    QPushButton* buttonok = new QPushButton( tr( "&OK" ) );
    connect( buttoncancel, SIGNAL( clicked() ), this, SLOT( close() ) );
    connect( buttonok, SIGNAL( clicked() ), this, SLOT( saveConfig() ) );
    connect( buttonok, SIGNAL( clicked() ), this, SLOT( close() ) );
    buttonbox->addWidget( buttoncancel );
    buttonbox->addWidget( buttonok );
    mainlayout->addLayout( buttonbox );
    setMinimumWidth( 530 );
}

void Config::saveConfig()
{
    QSettings settings;
    /// general
    settings.setValue( "/qsopcast/player_mp3", player_mp3->text() );
    settings.setValue( "/qsopcast/player_rmvb", player_rmvb->text() );
    settings.setValue( "/qsopcast/player_wmv", player_wmv->text() );
    settings.setValue( "/qsopcast/recorddir", recorddir->text() );
    /// sopcast
    settings.setValue( "/qsopcast/sopcast_channelurl", sopcast_channelurl->text() );
    settings.setValue( "/qsopcast/sopcast_bin", sopcast_binurl->text() );
//     settings.setValue( "/qsopcast/channelheader", editchannelheader->text() );
    settings.setValue( "/qsopcast/sopcast_forcetcp", sopcast_forcetcp->isChecked() );
    settings.setValue( "/qsopcast/sopcast_useexittime", sopcast_useexittime->isChecked() );
    if ( sopcast_useexittime->isChecked() )
        settings.setValue( "/qsopcast/sopcast_exittime", sopcast_exittime->value() );
    settings.setValue( "/qsopcast/sopcast_useauth", sopcast_useauth->isChecked() );
    if ( sopcast_useauth->isChecked() ) {
        settings.setValue( "/qsopcast/sopcast_username", sopcast_username->text() );
        settings.setValue( "/qsopcast/sopcast_password", sopcast_password->text() );
    }
    /// pplive
    settings.setValue( "/qsopcast/pplive_channelurl", pplive_channelurl->text() );
    settings.setValue( "/qsopcast/pplive_bin", pplive_binurl->text() );
    /// ppstream
    settings.setValue( "/qsopcast/ppstream_channelurl", ppstream_channelurl->text() );
    settings.setValue( "/qsopcast/ppstream_bin", ppstream_binurl->text() );

    /// bookmark
    settings.setValue( "/qsopcast/bookmark_fileurl", bookmark_fileurl->text() );
    /// history
    settings.setValue( "/qsopcast/history_fileurl", history_fileurl->text() );
    settings.setValue( "/qsopcast/history_maxcount", history_maxcount->value() );
}

#include "config.moc"
