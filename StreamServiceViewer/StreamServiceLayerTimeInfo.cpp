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


#include "StreamServiceLayerTimeInfo.h"

StreamServiceLayerTimeInfo::StreamServiceLayerTimeInfo(QObject *parent) : QObject(parent)
{

}

StreamServiceLayerTimeInfo* StreamServiceLayerTimeInfo::createFromJson(QJsonValue &timeInfoValue, QObject *parent)
{
    if (!timeInfoValue.isObject())
    {
        return nullptr;
    }

    auto layerTimeInfo = new StreamServiceLayerTimeInfo(parent);
    QJsonObject timeInfoObject = timeInfoValue.toObject();

    auto const trackIdFieldKey = "trackIdField";
    if (timeInfoObject.contains(trackIdFieldKey))
    {
        layerTimeInfo->m_trackIdField = timeInfoObject.value(trackIdFieldKey).toString();
    }

    auto const startTimeFieldKey = "startTimeField";
    if (timeInfoObject.contains(startTimeFieldKey))
    {
        layerTimeInfo->m_startTimeField = timeInfoObject.value(startTimeFieldKey).toString();
    }

    auto const endTimeFieldKey = "endTimeField";
    if (timeInfoObject.contains(endTimeFieldKey))
    {
        layerTimeInfo->m_endTimeField = timeInfoObject.value(endTimeFieldKey).toString();
    }

    return layerTimeInfo;
}

QString StreamServiceLayerTimeInfo::trackIdField() const
{
    return m_trackIdField;
}

QString StreamServiceLayerTimeInfo::startTimeField() const
{
    return m_startTimeField;
}

QString StreamServiceLayerTimeInfo::endTimeField() const
{
    return m_endTimeField;
}
