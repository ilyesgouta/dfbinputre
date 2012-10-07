// DFbInputRe, a tool for redirecting the local mouse and keyboard input
// to a remote target over the network using UDP datagrams.
//
// Copyright (C) 2012, Ilyes Gouta, ilyes.gouta@gmail.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QUdpSocket>
#include <QHostAddress>

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent, const QString& target);
    ~MainWindow();

public slots:
    void AboutBox();
    void ConnectBox();

private:
    Ui::MainWindow *ui;

    QUdpSocket *m_UdpSocket;

    QString m_target;
    QString m_targetIpAddr;
    int m_targetPort;

    bool m_connected;

    bool parseTargetAddress(const QString& address);

    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // MAINWINDOW_H
