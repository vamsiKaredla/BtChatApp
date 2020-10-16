#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "server.h"
#include "client.h"
#include <QQmlComponent>
#include<QObject>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    qmlRegisterType<Server>("com.company.server",1,0,"Server");
    qmlRegisterType<Client>("com.company.client",1,0,"Client");
    QQmlApplicationEngine engine; 
    QQmlComponent component(&engine,QUrl(QStringLiteral("qrc:/main.qml")));
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);




    return app.exec();
}
