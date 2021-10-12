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

#include <QJsonArray>
#include <QJsonDocument>

using namespace Esri::ArcGISRuntime;

RendererFactory::RendererFactory(QObject *parent) : QObject(parent)
{
}

Renderer* RendererFactory::createRendererFromDrawingInfo(const QJsonValue &drawingInfoValue)
{
    auto const rendererKey = "renderer";
    QJsonObject drawingInfoObject = drawingInfoValue.toObject();
    if (!drawingInfoObject.contains(rendererKey))
    {
        qDebug() << "Drawing info does not contain a renderer!";
        return nullptr;
    }

    QJsonValue rendererValue = drawingInfoObject.value(rendererKey);
    if (!rendererValue.isObject())
    {
        qDebug() << "Renderer does not represent an object!";
        return nullptr;
    }

    // Parsing the renderer object
    QJsonObject rendererObject = rendererValue.toObject();
    QJsonDocument rendererDocument(rendererObject);
    return Renderer::fromJson(rendererDocument.toJson(), this);
}

Renderer* RendererFactory::createHeatmapRenderer(GraphicListModel const *graphicsModel)
{
    QJsonObject rendererObject;
    rendererObject.insert("type", "heatmap");
    rendererObject.insert("blurRadius", 10);

    QJsonArray colorStopsArray;
    colorStopsArray.push_back(createColorStop(0, QColor(255,255,255,0)));
    colorStopsArray.push_back(createColorStop(0.2, QColor(255,255,255,1)));
    colorStopsArray.push_back(createColorStop(0.5, QColor(255,140,0,1)));
    colorStopsArray.push_back(createColorStop(0.8, QColor(255,140,0,1)));
    colorStopsArray.push_back(createColorStop(1, QColor(255,0,0,1)));

    // TODO: Colorstops do not work as expected!
    // https://developers.arcgis.com/web-map-specification/objects/heatmapRenderer/
    //rendererObject.insert("colorStops", colorStopsArray);
    rendererObject.insert("maxPixelIntensity", 50);
    rendererObject.insert("minPixelIntensity", 0);

    QJsonDocument rendererDocument(rendererObject);
    return Renderer::fromJson(rendererDocument.toJson(), this);
}

QJsonObject RendererFactory::createColorStop(double ratio, const QColor &color)
{
    QJsonObject colorStop;
    colorStop.insert("ratio", ratio);
    QJsonArray colorArray;
    colorArray.push_back(color.red());
    colorArray.push_back(color.green());
    colorArray.push_back(color.blue());
    colorArray.push_back(color.alpha());
    colorStop.insert("color", colorArray);
    return colorStop;
}
