#ifndef SERVER_H
#define SERVER_H
#include <QObject>
#include <QList>
#include <qqml.h>
#include <QtBluetooth/qbluetoothhostinfo.h>
#include <QtBluetooth/qbluetoothaddress.h>
#include <QtBluetooth/qbluetoothserviceinfo.h>
QT_FORWARD_DECLARE_CLASS(QBluetoothServer)
QT_FORWARD_DECLARE_CLASS(QBluetoothSocket)
class Server:public QObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    Server();
    Q_INVOKABLE void startServer(const QBluetoothAddress &localAdapter = QBluetoothAddress());
    Q_INVOKABLE void stopServer();
    Q_INVOKABLE void sendMessage(const QString &message);

   private:
    QBluetoothServer *rfcommServer = nullptr;
    QBluetoothServiceInfo serviceInfo;
    QBluetoothSocket* clientSocket;
    QList<QBluetoothHostInfo> localAdapters;
    QString localName;
signals:
    void clientConnected(const QString &name);
     void messageReceived(const QString &sender, const QString &message);
     void clientDiscon(const QString &name);
     void serverStopped();
private slots:

    void connected();
    void readSocket();
    void clientDisconnected();
    //void sendMessage(const QString &message);
};

#endif // SERVER_H
