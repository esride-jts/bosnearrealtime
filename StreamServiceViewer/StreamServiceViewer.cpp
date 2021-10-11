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
#include "StreamServiceLayer.h"

#include "Basemap.h"
#include "GraphicsOverlay.h"
#include "Map.h"
#include "MapQuickView.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleRenderer.h"

#include <QProcessEnvironment>
#include <QUrl>

using namespace Esri::ArcGISRuntime;

StreamServiceViewer::StreamServiceViewer(QObject* parent /* = nullptr */):
    QObject(parent),
    m_map(new Map(BasemapStyle::OsmStandard, this)),
    m_streamGraphicsOverlay(new GraphicsOverlay(this))
{
    // Define the stream service endpoint
    QString streamServiceEndpointKeyName = "streamservice_endpoint";
    QProcessEnvironment systemEnvironment = QProcessEnvironment::systemEnvironment();
    if (systemEnvironment.contains(streamServiceEndpointKeyName))
    {
        QString streamServiceEndpoint = systemEnvironment.value(streamServiceEndpointKeyName);
        m_streamServiceLayer = new StreamServiceLayer(QUrl(streamServiceEndpoint), this);
    }
    else
    {
        qWarning() << "No stream service endpoint configured!";
    }

    // Define the graphics rendering
    SimpleMarkerSymbol *streamMarkerSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, Qt::black, 5, this);
    SimpleRenderer *streamGraphicsRenderer = new SimpleRenderer(streamMarkerSymbol, this);
    m_streamGraphicsOverlay->setRenderer(streamGraphicsRenderer);
    m_streamGraphicsOverlay->setOpacity(0.65f);
}

StreamServiceViewer::~StreamServiceViewer()
{
}

MapQuickView* StreamServiceViewer::mapView() const
{
    return m_mapView;
}

// Set the view (created in QML)
void StreamServiceViewer::setMapView(MapQuickView* mapView)
{
    if (!mapView || mapView == m_mapView)
    {
        return;
    }

    m_mapView = mapView;
    m_mapView->setMap(m_map);

    // Add the graphics overlay
    // Define the target graphics model for the stream service layer
    m_mapView->graphicsOverlays()->append(m_streamGraphicsOverlay);
    m_streamServiceLayer->setGraphicsModel(m_streamGraphicsOverlay->graphics());

    emit mapViewChanged();
}

void StreamServiceViewer::subscribeEvents()
{
    // Start streaming
    m_streamServiceLayer->subscribe();
}

void StreamServiceViewer::unsubscribeEvents()
{
    // Stop streaming
    m_streamServiceLayer->unsubscribe();
}
