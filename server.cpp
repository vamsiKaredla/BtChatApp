#include "server.h"
#include <QtBluetooth/qbluetoothserver.h>
#include <QtBluetooth/qbluetoothsocket.h>
#include <QtBluetooth/qbluetoothdeviceinfo.h>
#include <QtBluetooth/qbluetoothlocaldevice.h>
#include <QtBluetooth/qbluetoothuuid.h>

static const QLatin1String serviceUuid("e8e10f95-1a70-4b27-9ccf-02010264e9c8");
Server::Server()
{
localAdapters = QBluetoothLocalDevice::allDevices();
localName=QBluetoothLocalDevice().name();

}

void Server::startServer(const QBluetoothAddress &localAdapter)
{
    qInfo("starting server");
    if(rfcommServer)
        return;
    rfcommServer =new QBluetoothServer(QBluetoothServiceInfo::RfcommProtocol, this);
    connect(rfcommServer,&QBluetoothServer::newConnection,this,&Server::connected);
    bool result = rfcommServer->listen(localAdapter);
    if (!result) {
        qWarning() << "Cannot bind chat server to" << localAdapter.toString();
        return;
    }

    QBluetoothServiceInfo::Sequence profileSequence;
    QBluetoothServiceInfo::Sequence classId;
    classId << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::SerialPort));
    classId << QVariant::fromValue(quint16(0x100));
    profileSequence.append(QVariant::fromValue(classId));
    serviceInfo.setAttribute(QBluetoothServiceInfo::BluetoothProfileDescriptorList,
                             profileSequence);

    classId.clear();
    classId << QVariant::fromValue(QBluetoothUuid(serviceUuid));
    classId << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::SerialPort));

    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceClassIds, classId);

    //! [Service name, description and provider]
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceName, tr("Bt Chat Server"));
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceDescription,
                             tr(" bluetooth chat server"));
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceProvider, tr("chatApps"));
    //! [Service name, description and provider]

    //! [Service UUID set]
    serviceInfo.setServiceUuid(QBluetoothUuid(serviceUuid));
    //! [Service UUID set]

    //! [Service Discoverability]
    QBluetoothServiceInfo::Sequence publicBrowse;
    publicBrowse << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::PublicBrowseGroup));
    serviceInfo.setAttribute(QBluetoothServiceInfo::BrowseGroupList,
                             publicBrowse);
    //! [Service Discoverability]

    //! [Protocol descriptor list]
    QBluetoothServiceInfo::Sequence protocolDescriptorList;
    QBluetoothServiceInfo::Sequence protocol;
    protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::L2cap));
    protocolDescriptorList.append(QVariant::fromValue(protocol));
    protocol.clear();
    protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::Rfcomm))
             << QVariant::fromValue(quint8(rfcommServer->serverPort()));
    protocolDescriptorList.append(QVariant::fromValue(protocol));
    serviceInfo.setAttribute(QBluetoothServiceInfo::ProtocolDescriptorList,
                             protocolDescriptorList);
    //! [Protocol descriptor list]

    //! [Register service]
    serviceInfo.registerService(localAdapter);
}

void Server::connected()
{
    /*if(clientSocket!=nullptr)
        return;*/
    QBluetoothSocket *socket = rfcommServer->nextPendingConnection();
    if (!socket)
        return;
    connect(socket,&QBluetoothSocket::readyRead,this,&Server::readSocket);
    connect(socket,&QBluetoothSocket::disconnected,this, QOverload<>::of(&Server::clientDisconnected));
   // clientSocket.append(socket);
    clientSocket = socket;
    qInfo("adgsjd");
    emit clientConnected(socket->peerName());
}


void Server::readSocket()
{
    QBluetoothSocket *socket = qobject_cast<QBluetoothSocket *>(sender());
    if (!socket)
        return;

    while (socket->canReadLine()) {
        QByteArray line = socket->readLine().trimmed();
        emit messageReceived(socket->peerName(),
                             QString::fromUtf8(line.constData(), line.length()));
    }
}

void Server::clientDisconnected()
{
QBluetoothSocket *socket = qobject_cast<QBluetoothSocket *>(sender());
if(!socket)
    return;
emit clientDiscon(socket->peerName());
//clientSocket.removeOne(socket);
clientSocket=nullptr;

socket->deleteLater();
}
void Server::stopServer()
{
    qInfo("server stopped");
    // Unregister service
    serviceInfo.unregisterService();

    // Close sockets
   //qDeleteAll(clientSocket);


    // Close server
    delete rfcommServer;
    rfcommServer = nullptr;
     emit serverStopped();
}

void Server::sendMessage(const QString &message)
{
    qDebug()<<message;
    QByteArray text = message.toUtf8()+'\n';
    clientSocket->write(text);
    /*for (QBluetoothSocket *socket : qAsConst(clientSocket))
        socket->write(text);*/
}
