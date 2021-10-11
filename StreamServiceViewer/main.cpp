// Copyright 2021 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

#include "StreamServiceViewer.h"

#include "ArcGISRuntimeEnvironment.h"
#include "MapQuickView.h"

#include <QDir>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>

//------------------------------------------------------------------------------

using namespace Esri::ArcGISRuntime;


#include <QProcessEnvironment>
static void readApiKeyFromEnvironment()
{
    QString apiKeyName = "arcgisruntime_api_key";
    QProcessEnvironment systemEnvironment = QProcessEnvironment::systemEnvironment();
    if (systemEnvironment.contains(apiKeyName))
    {
        QString apiKey = systemEnvironment.value(apiKeyName);
        ArcGISRuntimeEnvironment::setApiKey(apiKey);
    }
    else
    {
        qWarning() << "Use of Esri location services, including basemaps, requires "
            "you to authenticate with an ArcGIS identity or set the API Key property.";
    }
}


int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    // Use of Esri location services, including basemaps and geocoding, requires
    // either an ArcGIS identity or an API key. For more information see
    // https://links.esri.com/arcgis-runtime-security-auth.

    // 1. ArcGIS identity: An ArcGIS named user account that is a member of an
    // organization in ArcGIS Online or ArcGIS Enterprise.

    // 2. API key: A permanent key that gives your application access to Esri
    // location services. Create a new API key or access existing API keys from
    // your ArcGIS for Developers dashboard (https://links.esri.com/arcgis-api-keys).

    // Production deployment of applications built with ArcGIS Runtime requires you to
    // license ArcGIS Runtime functionality. For more information see
    // https://links.esri.com/arcgis-runtime-license-and-deploy.
    readApiKeyFromEnvironment();

    // ArcGISRuntimeEnvironment::setLicense("Place license string in here");

    // Register the map view for QML
    qmlRegisterType<MapQuickView>("Esri.StreamServiceViewer", 1, 0, "MapView");

    // Register the StreamServiceViewer (QQuickItem) for QML
    qmlRegisterType<StreamServiceViewer>("Esri.StreamServiceViewer", 1, 0, "StreamServiceViewer");

    // Activate the styling
    QQuickStyle::setStyle("Material");

    // Initialize application view
    QQmlApplicationEngine engine;

    // Add the import Path
    engine.addImportPath(QDir(QCoreApplication::applicationDirPath()).filePath("qml"));

    // Set the source
    engine.load(QUrl("qrc:/qml/main.qml"));

    return app.exec();
}

//------------------------------------------------------------------------------
