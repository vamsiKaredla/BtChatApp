#ifndef CLIENT_H
#define CLIENT_H
#include<QObject>
#include <qqml.h>
#include <QtBluetooth/qbluetoothaddress.h>
#include <QtBluetooth/qbluetoothserviceinfo.h>
#include <QtBluetooth/qbluetoothuuid.h>
#include <QtBluetooth/qbluetoothhostinfo.h>
#include <QtBluetooth/qbluetoothaddress.h>
#include <QtBluetooth/qbluetoothdeviceinfo.h>
#include <QtBluetooth/qbluetoothlocaldevice.h>
#include <QtBluetooth/qbluetoothsocket.h>
QT_FORWARD_DECLARE_CLASS(QBluetoothServiceDiscoveryAgent)
class Client : public QObject
{
    Q_OBJECT

public:
    Client();
    Q_INVOKABLE void clientMode();
    Q_INVOKABLE QVariantList getList();
    Q_INVOKABLE void sendIndex(QString index);
    Q_INVOKABLE void sendMessage(const QString &message);
    Q_INVOKABLE void stopClient();
    void searchService(const QBluetoothAddress &localAdapter);
    void startClient(const QBluetoothServiceInfo &remoteService);
private:
    QBluetoothServiceDiscoveryAgent *m_discoveryAgent;
    QBluetoothServiceInfo m_service;
    QList<QBluetoothHostInfo> localAdapters;
    QString localName;
    int currentAdapterIndex = 0;
    QMap<qint32,QBluetoothServiceInfo> m_discoveredServices;
    QBluetoothServiceInfo service;
    QVariantList servicelist;
    qint32 index=0;
    QBluetoothSocket *socket = nullptr;
signals:
    void discovered();
     void messageReceived(const QString &sender, const QString &message);
     void connected(const QString &name);
     void socketErrorOccurred();
     void clientStopped();
     void noService();
private slots:
    void serviceDiscovered(const QBluetoothServiceInfo &serviceInfo);
    void discoveryFinished();
    void readSocket();
    void connected();
    void disconnected();
    void onSocketErrorOccurred(QBluetoothSocket::SocketError error);

};

#endif // CLIENT_H
