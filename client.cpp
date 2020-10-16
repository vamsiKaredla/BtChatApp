#include "client.h"
#include <QtBluetooth/qbluetoothlocaldevice.h>
#include <QtBluetooth/qbluetoothservicediscoveryagent.h>
static const QLatin1String serviceUuid("e8e10f95-1a70-4b27-9ccf-02010264e9c8");
Client::Client()
{
    localAdapters = QBluetoothLocalDevice::allDevices();
    localName=QBluetoothLocalDevice().name();

}

void Client::clientMode()
{
    qInfo("client started");
    const QBluetoothAddress adapter = localAdapters.isEmpty() ?
                                           QBluetoothAddress() :
                                           localAdapters.at(currentAdapterIndex).address();
    Client::searchService(adapter);
}

void Client::searchService(const QBluetoothAddress &localAdapter)
{
    qInfo("search for service");
    m_discoveryAgent = new QBluetoothServiceDiscoveryAgent(localAdapter);
    connect(m_discoveryAgent, SIGNAL(serviceDiscovered(QBluetoothServiceInfo)),
            this, SLOT(serviceDiscovered(QBluetoothServiceInfo)));
    connect(m_discoveryAgent, SIGNAL(finished()), this, SLOT(discoveryFinished()));
    m_discoveryAgent->setUuidFilter(QBluetoothUuid(serviceUuid));
    m_discoveryAgent->start(QBluetoothServiceDiscoveryAgent::FullDiscovery);
    qInfo("starting discovery");
}
void Client::serviceDiscovered(const QBluetoothServiceInfo &serviceInfo)
{
    qInfo("service discovered");
#if 0
    qDebug() << "Discovered service on"
             << serviceInfo.device().name() << serviceInfo.device().address().toString();
    qDebug() << "\tService name:" << serviceInfo.serviceName();
    qDebug() << "\tDescription:"
             << serviceInfo.attribute(QBluetoothServiceInfo::ServiceDescription).toString();
    qDebug() << "\tProvider:"
             << serviceInfo.attribute(QBluetoothServiceInfo::ServiceProvider).toString();
    qDebug() << "\tL2CAP protocol service multiplexer:"
             << serviceInfo.protocolServiceMultiplexer();
    qDebug() << "\tRFCOMM server channel:" << serviceInfo.serverChannel();
#endif
    const QBluetoothAddress address = serviceInfo.device().address();
    for (const QBluetoothServiceInfo &info : qAsConst(m_discoveredServices)) {
        if (info.device().address() == address)
            return;
    }

    QString remoteName;
    if (serviceInfo.device().name().isEmpty())
        remoteName = address.toString();
    else
        remoteName = serviceInfo.device().name();


    m_discoveredServices.insert(index,serviceInfo);
    servicelist.append(remoteName);

    index++;
    qDebug()<<remoteName;

}

void Client::discoveryFinished()
{
    qDebug()<<servicelist.length();
    if(servicelist.length()>0)
    {

    emit discovered();
    }
    else
    {
        emit noService();
    }
}

QVariantList Client::getList()
{
    return servicelist;
}

void Client::sendIndex(QString index)
{
    qint32 ind=index.toInt();
    m_service=m_discoveredServices[ind-1];
    Client::startClient(m_service);
}
void Client::startClient(const QBluetoothServiceInfo &remoteService)
{
    if (socket)
        return;

    // Connect to service
    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
    qDebug() << "Create socket";
    socket->connectToService(remoteService);
    qDebug() << "ConnectToService done";

    connect(socket, &QBluetoothSocket::readyRead, this, &Client::readSocket);
    connect(socket, &QBluetoothSocket::connected, this, QOverload<>::of(&Client::connected));
    connect(socket, &QBluetoothSocket::disconnected, this, &Client::disconnected);
    connect(socket, QOverload<QBluetoothSocket::SocketError>::of(&QBluetoothSocket::error),
            this, &Client::onSocketErrorOccurred);

}
void Client::stopClient()
{
    delete socket;
    socket = nullptr;
    m_discoveryAgent=nullptr;
    m_discoveredServices.clear();
    servicelist.clear();

}
//! [stopClient]

//! [readSocket]
void Client::readSocket()
{
    if (!socket)
        return;
    qInfo("message recieved");
    /*while (socket->canReadLine()) {
        QByteArray line = socket->readLine().trimmed();
        emit messageReceived(socket->peerName(),
                             QString::fromUtf8(line.constData(), line.length()));*/
        while (socket->canReadLine()) {
            QByteArray line = socket->readLine();
            emit messageReceived(socket->peerName(),
                                 QString::fromUtf8(line.constData(), line.length()));
    }
}
//! [readSocket]

//! [sendMessage]
void Client::sendMessage(const QString &message)
{
    QByteArray text = message.toUtf8() + '\n';
    socket->write(text);
}
//! [sendMessage]

void Client::onSocketErrorOccurred(QBluetoothSocket::SocketError error)
{
    if (error == QBluetoothSocket::NoSocketError)
        return;

    QMetaEnum metaEnum = QMetaEnum::fromType<QBluetoothSocket::SocketError>();
    QString errorString = socket->peerName() + QLatin1Char(' ')
            + metaEnum.valueToKey(error) + QLatin1String(" occurred");

    emit socketErrorOccurred();
}
void Client::connected()
{
    emit connected(socket->peerName());
}
void Client::disconnected()
{
    qInfo("disconnected");
    emit clientStopped();
}
