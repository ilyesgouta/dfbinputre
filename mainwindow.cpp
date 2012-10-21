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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMouseEvent>
#include <QInputDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent, const QString& target) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("DFbInputRe");

    connect(ui->action_About, SIGNAL(activated()), this, SLOT(AboutBox()));
    connect(ui->action_Connect, SIGNAL(activated()), this, SLOT(ConnectBox()));

    m_target = target;

    m_UdpSocket = 0;
    m_connected = false;

    m_mouseAcquired = false;

    ui->action_Connect->setText("Connect...");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ConnectBox()
{
    bool ok;

    if (m_connected) {

        delete m_UdpSocket;

        m_UdpSocket = 0;
        m_connected = false;

        m_mouseAcquired = false;

        releaseMouse();

        ui->action_Connect->setText("Connect...");
    } else {
        QInputDialog *input = new QInputDialog(this);

        QString result = input->getText(this, "Target UDP port",
                                              "Target UDP port",
                                              QLineEdit::Normal,
                                              m_target, &ok);

        delete input;

        if (!ok || result.isEmpty())
            return;

        if (!parseTargetAddress(result))
            return;

        m_UdpSocket = new QUdpSocket();
        m_connected = true;

        m_mouseAcquired = false;

        releaseMouse();

        ui->action_Connect->setText("Disconnect");
    }
}

bool MainWindow::parseTargetAddress(const QString& address)
{
    bool ok;

    QString targetIpAddr = address.section(':', 0, 0);
    unsigned int targetPort = address.section(':', 1, 1).toInt(&ok);

    if (!ok)
        return false;

    QStringList list = targetIpAddr.split(".");
    if (list.length() != 4)
        return false;

    m_targetPort = targetPort;
    m_targetAddr.setAddress(targetIpAddr);

    return true;
}

void MainWindow::AboutBox()
{
    QMessageBox::about(this, "DFbInputRe", "A tool for redirecting local mouse and keyboard input "
                       "to DirectFB over the network. Written by Ilyes Gouta.");
}

int MainWindow::writeDatagram(DFbInputType type, unsigned int param0, unsigned int param1, unsigned int param2)
{
    DFbInputPacket packet;

    packet.magic = MAGIC;
    packet.type = type;
    packet.keyascii = param0;
    packet.cursor[0] = param1;
    packet.cursor[1] = param2;

    return m_UdpSocket->writeDatagram(reinterpret_cast<char*>(&packet), sizeof(packet), m_targetAddr, m_targetPort);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (m_mouseAcquired) {
        if (event->modifiers() & Qt::ControlModifier) {
            releaseMouse();
            m_mouseAcquired = false;

            ui->statusBar->showMessage("Mouse released");

            qApp->removeEventFilter(this);
        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (m_connected && !m_mouseAcquired) {
        grabMouse();
        m_mouseAcquired = true;

        ui->statusBar->showMessage("Mouse acquired");

        qApp->installEventFilter(this);
    }
}

#define UNUSED_PARAMETER(a) (a) = (a)

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    QKeyEvent *kevent;
    QMouseEvent *mevent;
    QString position;

    static bool interior = false;

    UNUSED_PARAMETER(obj);

    switch (event->type()) {
    case QEvent::MouseMove:
        mevent = static_cast<QMouseEvent *>(event);

        interior = frameGeometry().contains(mevent->globalPos());

        if (m_mouseAcquired) {
            position = QString::number(mevent->globalX()) + "," + QString::number(mevent->globalY());
            ui->statusBar->showMessage("Mouse move " + position);

            writeDatagram(DFBINPUT_MOUSE_MOTION, 0, mevent->globalX(), mevent->globalY());
        }
        break;
    case QEvent::KeyPress:
        kevent = static_cast<QKeyEvent *>(event);
        if (m_mouseAcquired) {
            // Forward the event to the MainWindow
            if (kevent->modifiers() & Qt::ControlModifier)
                return false;

            ui->statusBar->showMessage("Key press " + kevent->text());

            if (kevent->text().length() > 0)
                writeDatagram(DFBINPUT_KEYPRESS, kevent->text().at(0).toAscii(), 0, 0);
        }
        break;
    case QEvent::MouseButtonPress:
        if (m_mouseAcquired) {
            mevent = static_cast<QMouseEvent *>(event);

            position = QString::number(mevent->globalX()) + "," + QString::number(mevent->globalY());
            ui->statusBar->showMessage("Mouse click at " + position);

            writeDatagram(DFBINPUT_MOUSE_CLICK, 0, mevent->globalX(), mevent->globalY());
        }
        break;
    case QEvent::MouseButtonDblClick:
        if (m_mouseAcquired) {
            mevent = static_cast<QMouseEvent *>(event);

            position = QString::number(mevent->globalX()) + "," + QString::number(mevent->globalY());
            ui->statusBar->showMessage("Mouse double-click at " + position);

            writeDatagram(DFBINPUT_MOUSE_DBLCLICK, 0, mevent->globalX(), mevent->globalY());
        }
        break;
    case QEvent::Leave:
        if (m_mouseAcquired && interior)
            grabMouse();
        break;
    default:
        return false;
    }

    return true;
}
