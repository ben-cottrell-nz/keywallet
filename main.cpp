#include <QQmlContext>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QVector>
#include "kw_session.h"

void populate_creds(credential_store* model,
                    QVector<credential_store::entry> items)
{
    model->entries() = items;
}

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    kw_session session;
    populate_creds(session.credential_model(),
                   QVector<credential_store::entry>{ {"facebook","xxx"},
                     {"gmail","xxx"},
                     {"ubisoft","fff"}});
    engine.rootContext()->setContextProperty("kw_session", &session);
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
