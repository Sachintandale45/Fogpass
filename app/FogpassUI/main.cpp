#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "src/backend/backendmanager.h"

int main(int argc, char *argv[])
{
    // Do not force the virtual keyboard module here; avoid runtime import warnings

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    BackendManager backend;
    engine.rootContext()->setContextProperty("Backend", &backend); // Use "Backend" to match QML
    backend.init(); // Request initial state from services
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("trial1", "Main");

    return app.exec();
}
