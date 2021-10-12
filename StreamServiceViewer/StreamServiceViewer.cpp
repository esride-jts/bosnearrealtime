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

// StreamServiceViewer
// Copyright (C) 2021 Esri Deutschland GmbH
// Jan Tschada (j.tschada@esri.de)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Additional permission under GNU GPL version 3 section 7
//
// If you modify this Program, or any covered work, by linking or combining
// it with ArcGIS Runtime for Qt (or a modified version of that library),
// containing parts covered by the terms of ArcGIS Runtime for Qt,
// the licensors of this Program grant you additional permission to convey the resulting work.
// See <https://developers.arcgis.com/qt/> for further information.


#include "RendererFactory.h"
#include "StreamServiceViewer.h"
#include "StreamServiceLayer.h"

#include "Basemap.h"
#include "GraphicsOverlay.h"
#include "Map.h"
#include "MapQuickView.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleRenderer.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QProcessEnvironment>
#include <QUrl>

using namespace Esri::ArcGISRuntime;

StreamServiceViewer::StreamServiceViewer(QObject* parent /* = nullptr */):
    QObject(parent),
    m_map(new Map(BasemapStyle::OsmStandard, this)),
    m_streamGraphicsOverlay(new GraphicsOverlay(this)),
    m_networkAccessManager(new QNetworkAccessManager(this)),
    m_rendererFactory(new RendererFactory(this))
{
    // Listen to network replies
    connect(m_networkAccessManager, &QNetworkAccessManager::finished, this, &StreamServiceViewer::onStreamServiceInfoRequestFinished);

    // Define the stream service endpoint
    QString streamServiceEndpointKeyName = "streamservice_endpoint";
    QProcessEnvironment systemEnvironment = QProcessEnvironment::systemEnvironment();
    if (systemEnvironment.contains(streamServiceEndpointKeyName))
    {
        // Request the stream service json description
        QString streamServiceEndpoint = systemEnvironment.value(streamServiceEndpointKeyName);
        QUrl streamServiceInfoEndpoint(streamServiceEndpoint);
        streamServiceInfoEndpoint.setQuery("f=json");
        QNetworkRequest streamServiceInfoRequest(streamServiceInfoEndpoint);
        m_networkAccessManager->get(streamServiceInfoRequest);
    }
    else
    {
        qWarning() << "No stream service endpoint configured!";
    }
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
    m_mapView->graphicsOverlays()->append(m_streamGraphicsOverlay);

    emit mapViewChanged();
}

void StreamServiceViewer::subscribeEvents()
{
    if (nullptr == m_streamServiceLayer)
    {
        qWarning() << "Stream service layer was not initialized!";
        return;
    }

    // Start streaming
    m_streamServiceLayer->subscribe();
}

void StreamServiceViewer::unsubscribeEvents()
{
    if (nullptr == m_streamServiceLayer)
    {
        qWarning() << "Stream service layer was not initialized!";
        return;
    }

    // Stop streaming
    m_streamServiceLayer->unsubscribe();
}

void StreamServiceViewer::renderSimple()
{
    m_streamGraphicsOverlay->setRenderer(m_simpleRenderer);
}

void StreamServiceViewer::renderHeat()
{
    m_streamGraphicsOverlay->setRenderer(m_heatmapRenderer);
}

void StreamServiceViewer::onStreamServiceInfoRequestFinished(QNetworkReply *infoReply)
{
    if (infoReply->error())
    {
        qWarning() << "Stream service info request failed!";
        return;
    }

    // Parse the service description
    QJsonDocument serviceDocument = QJsonDocument::fromJson(infoReply->readAll());
    if (serviceDocument.isNull())
    {
        qDebug() << "Unsupported service description received!";
        return;
    }

    if (!serviceDocument.isObject())
    {
        qDebug() << "Service description does not represent an object!";
        return;
    }

    // TODO: Parse the whole json!

    auto const drawingInfoKey = "drawingInfo";
    QJsonObject serviceObject = serviceDocument.object();
    if (!serviceObject.contains(drawingInfoKey))
    {
        qDebug() << "Service description does not contain a drawing info!";
        return;
    }

    QJsonValue drawingInfoValue = serviceObject.value(drawingInfoKey);
    if (!drawingInfoValue.isObject())
    {
        qDebug() << "Drawing info does not represent an object!";
        return;
    }



    auto const streamUrlsKey = "streamUrls";
    if (!serviceObject.contains(streamUrlsKey))
    {
        qDebug() << "Service description does not contain any streaming urls!";
        return;
    }

    QJsonValue streamUrlsValue = serviceObject.value(streamUrlsKey);
    if (!streamUrlsValue.isArray())
    {
        qDebug() << "Streaming urls do not represent an array!";
        return;
    }

    QJsonArray streamUrlsArray = streamUrlsValue.toArray();
    if (streamUrlsArray.size() < 1)
    {
        qDebug() << "Streaming urls does not contain any item!";
        return;
    }

    // TODO: Take all items not only the first one
    QJsonValue streamUrlsFirstItemValue = streamUrlsArray[0];
    if (!streamUrlsFirstItemValue.isObject())
    {
        qDebug() << "The first streaming urls item does not represent an object!";
        return;
    }

    // TODO: Parse the transport value ("ws" expected)
    auto const urlsKey = "urls";
    QJsonObject streamUrlsFirstItemObject = streamUrlsFirstItemValue.toObject();
    if (!streamUrlsFirstItemObject.contains(urlsKey))
    {
        qDebug() << "Streaming urls does not contain any urls!";
        return;
    }

    QJsonValue urlsValue = streamUrlsFirstItemObject.value(urlsKey);
    if (!urlsValue.isArray())
    {
        qDebug() << "Urls do not represent an array!";
        return;
    }

    QJsonArray urlsArray = urlsValue.toArray();
    if (urlsArray.size() < 1)
    {
        qDebug() << "Urls does not contain any item!";
        return;
    }

    // TODO: Take all urls not only the first url
    QString streamServiceWebSocketEndpoint = urlsArray[0].toString();
    qDebug() << "Web socket endpoint is " << streamServiceWebSocketEndpoint;
    m_streamServiceLayer = new StreamServiceLayer(QUrl(streamServiceWebSocketEndpoint), this);

    // Define the graphics rendering
    /*
    SimpleMarkerSymbol *streamMarkerSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, Qt::black, 5, this);
    SimpleRenderer *streamGraphicsRenderer = new SimpleRenderer(streamMarkerSymbol, this);
    */
    m_simpleRenderer = m_rendererFactory->createRendererFromDrawingInfo(drawingInfoValue);
    m_heatmapRenderer = m_rendererFactory->createHeatmapRenderer(m_streamGraphicsOverlay->graphics());
    m_streamGraphicsOverlay->setRenderer(m_simpleRenderer);
    m_streamGraphicsOverlay->setOpacity(0.85f);

    // Define the target graphics model for the stream service layer
    m_streamServiceLayer->setGraphicsModel(m_streamGraphicsOverlay->graphics());
}
