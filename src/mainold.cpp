// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QGuiApplication>
#include <QQmlApplicationEngine>

// #include "app_environment.h"
// #include "import_qml_components_plugins.h"
// #include "import_qml_plugins.h"
#include "src/treemodel.h"
#include "src/nettoolitemmodel.h"
int main(int argc, char *argv[])
{
    //set_qt_environment();

    QGuiApplication app(argc, argv);
    qmlRegisterType<NetToolItemModel>("MyControl.TreeModel", 1, 0, "NetToolItemModel");
    qmlRegisterType<TreeModel>("MyControl.TreeModel", 1, 0, "TreeModel");

    QQmlApplicationEngine engine("qrc:/qml/App.qml");
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    return app.exec();
}
