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

    setWindowTitle("DfbInputCapture");

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
    QMessageBox::about(this, "DfbInputCapture", "A tool for redirecting local mouse and keyboard input "
                       "to DirectFB over the network. Written by Ilyes Gouta.");
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    static bool lowBoundary = false;

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
