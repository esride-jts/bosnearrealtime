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
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Additional permission under GNU GPL version 3 section 7
//
// If you modify this Program, or any covered work, by linking or combining
// it with ArcGIS Runtime for Qt (or a modified version of that library),
// containing parts covered by the terms of ArcGIS Runtime for Qt,
// the licensors of this Program grant you additional permission to convey the resulting work.
// See <https://developers.arcgis.com/qt/> for further information.

#include "StreamServiceLayer.h"

#include "Feature.h"
#include "Geometry.h"
#include "GeometryEngine.h"
#include "Graphic.h"

#include <QJsonDocument>
#include <QJsonObject>

using namespace Esri::ArcGISRuntime;

StreamServiceLayer::StreamServiceLayer(const QUrl &webSocketEndpoint, QObject *parent) : QObject(parent),
    m_webSocketEndpoint(webSocketEndpoint)
{
    // Listen to the websocket signals
    connect(&m_websocket, &QWebSocket::connected, this, &StreamServiceLayer::onConnected);
    connect(&m_websocket, &QWebSocket::disconnected, this, &StreamServiceLayer::onDisconnected);
    connect(&m_websocket, &QWebSocket::binaryMessageReceived, this, &StreamServiceLayer::onBinaryMessageReceived);
    connect(&m_websocket, &QWebSocket::textMessageReceived, this, &StreamServiceLayer::onTextMessageReceived);
}

void StreamServiceLayer::subscribe()
{
    // Open the public accessible websocket
    QUrl subscribeEndpoint(m_webSocketEndpoint);
    subscribeEndpoint.setPath(subscribeEndpoint.path() + QStringLiteral("/subscribe"));
    m_websocket.open(subscribeEndpoint);
}

void StreamServiceLayer::unsubscribe()
{
    m_websocket.close();
}

void StreamServiceLayer::setGraphicsModel(Esri::ArcGISRuntime::GraphicListModel *graphicsModel)
{
    m_graphicsModel = graphicsModel;
}

void StreamServiceLayer::onConnected()
{
    qDebug() << "Websocket connected...";
}

void StreamServiceLayer::onDisconnected()
{
    qDebug() << "Websocket disconnected...";
}

void StreamServiceLayer::onBinaryMessageReceived(const QByteArray &message)
{
    qDebug() << "Websocket binary message received...";
}

void StreamServiceLayer::onTextMessageReceived(const QString &message)
{
    //qDebug() << "Websocket text message received...";
    //qDebug() << message;

    // We expect UTF-8 encoded messages here
    QJsonDocument featureDocument = QJsonDocument::fromJson(message.toUtf8());
    if (featureDocument.isNull())
    {
        qDebug() << "Unsupported text message received!";
        return;
    }

    if (!featureDocument.isObject())
    {
        qDebug() << "Text message does not represent an object!";
        return;
    }

    auto const geometryKey = "geometry";
    QJsonObject featureObject = featureDocument.object();
    if (!featureObject.contains(geometryKey))
    {
        qDebug() << "Text message does not represent a feature having a geometry!";
        return;
    }

    // Obtain the geometry object
    QJsonValue geometryValue = featureObject.value(geometryKey);
    if (!geometryValue.isObject())
    {
        qDebug() << "Text message does not represent a feature having a geometry object!";
        return;
    }

    // Parse the geometry object
    QJsonObject geometryObject = geometryValue.toObject();
    QJsonDocument geometryDocument(geometryObject);
    Geometry constructedGeometry = Geometry::fromJson(geometryDocument.toJson());
    if (constructedGeometry.isEmpty())
    {
        qDebug() << "Text message does not represent a feature having a valid geometry!";
        return;
    }

    if (nullptr == m_graphicsModel)
    {
        return;
    }

    // Add a new graphic using the constructed geometry
    switch (constructedGeometry.geometryType())
    {
        default:
            m_graphicsModel->append(new Graphic(constructedGeometry, this));
        break;
    }
}
