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

#include <QProcess>
#include <QSettings>

#include "abstractchannel.h"// for media format type enum
#include "utils.h"

#include "playfork.h"

PlayFork::PlayFork( QObject* parent ) :
        QObject( parent )
{
    player = 0;
}

PlayFork::~PlayFork()
{
    killPlayer();
}

void PlayFork::forkPlayer( const QString& url, format_t format )
{
    m_url = url;
    player = new QProcess( this );
//     connect( player, SIGNAL( processExited() ), this, SLOT( onPlayerExit() ) );
    QString playerapp;
    QSettings settings;
    switch ( format ) {
        case mp3:
            playerapp = settings.value( "/qsopcast/player_mp3", "xmms" ).toString();
            break;
        case rmvb:
            playerapp = settings.value( "/qsopcast/player_rmvb", "mplayer" ).toString();
            break;
        case wmv:
            playerapp = settings.value( "/qsopcast/player_wmv", "mplayer" ).toString();
            break;
    }
    QStringList args;
    args << m_url;
    player->start( playerapp, args );
}

////should be executed before executing detachSop, otherwise may fail
void PlayFork::killPlayer()
{
    if ( player ) {
    ///FIXME: sometimes fails to kill
//   if (!player->isRunning ())
//      return;
    ///
//     if ( !pageplay->sopfork )
//         return;

    ///the following code should be called before destructing TabWidget?
    ///find process id scope
//         long idmin = player->pid() + 20;
//     for ( QWidget * wd = pageplay->sopfork->listpage.first(); wd;
//             wd = pageplay->sopfork->listpage.next() )
        ///outdated: WARNING: here tabwidget should not be
        ///substituted by parentWidget(),
        ///because parentWidget() will lose infor during destruction
//         if ( static_cast < MyHBox * >( wd )->pagetype == "play" ) {
//             Q3Process *proc = static_cast < PagePlay * >( wd )
//                               ->playfork->player;
//             if ( proc->isRunning()
//                     && proc->processIdentifier() > player->processIdentifier()
//                     && proc->processIdentifier() < idmin )
//                 idmin = proc->processIdentifier();
//         }

    //kill all process within the scope
        killProcess( qPrintable(m_url), player->pid(), player->pid()+20/*idmin*/ );//FIXME: port


        player->close();
        delete player;
        player = 0;
    }
}

void PlayFork::onPlayerExit()
{
    ///restore button player color to black
//     pageplay->buttonplayer->setPaletteForegroundColor( Qt::black );
    ///restart player
//     if ( pageplay->buttonplayer->isOn() ) {
//         if ( static_cast < MainWindow * >( qApp->mainWidget() )->
//                 menubar->menu_config->isItemChecked( AUTO_RESTART_PLAYER ) ) {
//             pageplay->onButtonPlayerToggled( true );
//         }
//         else {
//             pageplay->buttonplayer->setOn( false );
//         }
//     }
}

#include "playfork.moc"
