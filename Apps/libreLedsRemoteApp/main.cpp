#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSettings>
#include <QQuickStyle>

#include "ledcontrolerserver.h"

int main(int argc, char *argv[])
{
    QGuiApplication::setApplicationName("libreLedsRemote");
    QGuiApplication::setOrganizationName("SuccessByFailure");
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    ledControlerServer  ledServer;
    ledServer.listen();

    QSettings settings;
    QString style = QQuickStyle::name();
    if (!style.isEmpty())
        settings.setValue("style", style);
    else
        QQuickStyle::setStyle(settings.value("style").toString());

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("ledServer", &ledServer);
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;
    return app.exec();
}
