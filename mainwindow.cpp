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
        qApp->removeEventFilter(this);

        delete m_UdpSocket;

        m_UdpSocket = 0;
        m_connected = false;

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

        m_target = result;

        m_UdpSocket = new QUdpSocket();
        m_connected = true;

        qApp->installEventFilter(this);

        ui->action_Connect->setText("Disconnect");
    }
}

bool MainWindow::parseTargetAddress(const QString& address)
{
    bool ok;

    QString targetIpAddr = address.section(':', 0, 0);
    address.section(':', 1, 1).toInt(&ok);

    if (!ok)
        return false;

    QStringList list = targetIpAddr.split(".");
    if (list.length() != 4)
        return false;

    return true;
}

void MainWindow::AboutBox()
{
    QMessageBox::about(this, "DFbInputRe", "A tool for redirecting local mouse and keyboard input "
                       "to DirectFB over the network. Written by Ilyes Gouta.");
}

#define UNUSED_PARAMETER(a) (a) = (a)

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    static bool lowBoundary = false;

    UNUSED_PARAMETER(obj);

    if (event->type() == QEvent::MouseMove) {
        QMouseEvent *e = static_cast<QMouseEvent *>(event);

        QString position = QString::number(e->globalX()) + "," + QString::number(e->globalY());
        ui->statusBar->showMessage("Mouse move " + position);

        lowBoundary = e->y() < 32;
    }

    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *e = static_cast<QKeyEvent *>(event);
        if (e->modifiers() & Qt::ControlModifier) {
            releaseMouse();
            return true;
        } else {
            ui->statusBar->showMessage("Key press " + e->text());
        }
    }

    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *e = static_cast<QMouseEvent *>(event);

        QString position = QString::number(e->globalX()) + "," + QString::number(e->globalY());
        ui->statusBar->showMessage("Mouse click at " + position);
    }

    if (event->type() == QEvent::Leave) {
        if (!lowBoundary)
            grabMouse();
    }

    return false;
}
