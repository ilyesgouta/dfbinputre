#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMouseEvent>
#include <QInputDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("DfbInputCapture");

    connect(ui->action_About, SIGNAL(activated()), this, SLOT(AboutBox()));
    connect(ui->action_Connect, SIGNAL(activated()), this, SLOT(ConnectBox()));

    qApp->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ConnectBox()
{
    bool ok;
    QInputDialog *input = new QInputDialog(this);

    QString result = input->getText(this, "Target UDP port",
                                          "Target UDP port",
                                          QLineEdit::Normal,
                                          "10.157.6.141:5000", &ok);

    delete input;

    if (!ok || result.isEmpty())
        return;

    QString targetIpAddr = result.section(':', 0, 0);
    int targetPort = result.section(':', 1, 1).toInt(&ok);

    if (!ok)
        targetPort = 5000;

    QStringList list = targetIpAddr.split(".");
    if (list.length() != 4)
        return;
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
