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

#ifndef CONFIG_H
#define CONFIG_H

#include <QDialog>

class QLineEdit;
class QCheckBox;
class QSpinBox;
class QPushButton;

class Config: public QDialog
{
    Q_OBJECT
    public:
        Config( QWidget* parent = 0 );
    private:
        /// player
        QLineEdit* player_mp3;
        QLineEdit* player_rmvb;
        QLineEdit* player_wmv;
        QLineEdit* recorddir;
        /// sopcast cfg
        QLineEdit* sopcast_channelurl;
        QLineEdit* sopcast_binurl;
        QCheckBox* sopcast_forcetcp;
        QCheckBox* sopcast_useexittime;
        QSpinBox* sopcast_exittime;
        QCheckBox* sopcast_useauth;
        QLineEdit* sopcast_username;
        QLineEdit* sopcast_password;
        /// pplive cfg
        QLineEdit* pplive_channelurl;
        QLineEdit* pplive_binurl;
        /// ppstream cfg
        QLineEdit* ppstream_channelurl;
        QLineEdit* ppstream_binurl;
        QPushButton* ppstream_clearcache;

        /// bookmark cfg
        QLineEdit* bookmark_fileurl;
        /// history cfg
        QLineEdit* history_fileurl;
        QSpinBox* history_maxcount;

    private slots:
        void saveConfig();
};

#endif // CONFIG_H
