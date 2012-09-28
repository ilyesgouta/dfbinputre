#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMouseEvent>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("DfbInputCapture");

    connect(ui->action_About, SIGNAL(activated()), this, SLOT(AboutBox()));

    qApp->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete ui;
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
