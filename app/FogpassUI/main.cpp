#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "src/backend/backendmanager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // 1. Create the C++ backend object first.
    BackendManager backend;

    // 2. Register any custom types that will be passed in signals to QML.
    // This is crucial for QStringList to work correctly.
    qRegisterMetaType<QStringList>();

    // 3. Create the QML engine.
    QQmlApplicationEngine engine;

    // 4. Expose the C++ backend object to the QML context under the name "Backend".
    engine.rootContext()->setContextProperty("Backend", &backend);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    // 5. Now that everything is set up, load the main QML file.
    engine.loadFromModule("trial1", "Main");

    // 6. Start any background tasks after the UI is loaded.
    backend.start(); // Start the battery simulation timer

    return app.exec();
}
