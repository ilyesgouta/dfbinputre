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
