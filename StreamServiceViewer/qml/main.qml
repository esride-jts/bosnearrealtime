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

import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.11
import Esri.StreamServiceViewer 1.0

ApplicationWindow {
    visible: true
    width: 800
    height: 600
    title: "Stream Service Viewer"

    Material.theme: Material.Dark
    Material.accent: "#a7ad6d"      // BW Hellgrün
    //Material.accent: "#616847"      // BW Helloliv
    Material.background: "#312d2a"  // BW Schwarz
    Material.foreground: "#d3c2a6"  // BW Beige
    Material.primary: "#434a39"     // BW Dunkelgrün

    header: ToolBar {
       ColumnLayout {
           anchors.fill: parent

           RowLayout {

              ButtonGroup {
                  buttons: [subscribeButton, unsubscribeButton]
              }

              RadioButton {
                  id: subscribeButton
                  text: qsTr("Subscribe")
                  onClicked: {
                    viewerFrom.subscribeEvents();
                  }
              }

              RadioButton {
                  id: unsubscribeButton
                  text: qsTr("Unsubscribe")
                  checked: true
                  onClicked: {
                    viewerFrom.unsubscribeEvents();
                  }
              }

              ButtonGroup {
                  buttons: [subscribeButton, unsubscribeButton]
              }

              RadioButton {
                  id: renderSimpleButton
                  text: qsTr("Simple")
                  checked: true
                  onClicked: {
                    viewerFrom.renderSimple();
                  }
              }

              RadioButton {
                  id: renderHeatMapButton
                  text: qsTr("Heat")
                  onClicked: {
                    viewerFrom.renderHeat();
                  }
              }
           }
       }
    }

    StreamServiceViewerForm {
        id: viewerFrom
        anchors.fill: parent
    }
}
