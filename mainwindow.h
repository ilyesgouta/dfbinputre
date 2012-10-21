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

#define MAGIC 0xc001feed

typedef enum {
    DFBINPUT_MOUSE_MOTION = 1,
    DFBINPUT_MOUSE_CLICK,
    DFBINPUT_MOUSE_DBLCLICK,
    DFBINPUT_KEYPRESS
} DFbInputType;

typedef struct DFbInputPacket {
    unsigned int magic;
    DFbInputType type;
    unsigned int keyascii;
    unsigned int cursor[2];
} DFbInputPacket __attribute__((packed));

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

    unsigned int m_targetPort;
    QHostAddress m_targetAddr;

    bool m_connected;

    bool m_mouseAcquired;

    bool parseTargetAddress(const QString& address);
    int writeDatagram(DFbInputType type, unsigned int param0, unsigned int param1, unsigned int param2);

    bool eventFilter(QObject *obj, QEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
};

#endif // MAINWINDOW_H
