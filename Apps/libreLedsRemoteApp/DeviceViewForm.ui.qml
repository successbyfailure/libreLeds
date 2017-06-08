import QtQuick 2.4
import QtQuick.Controls 2.0
import Qt.labs.settings 1.0
import QtQuick.Controls.Material 2.0
import QtQuick.Controls.Universal 2.0
import QtQuick.Layouts 1.3
import QtQuick.Extras 1.4
import QtQuick.Window 2.2

Item {
    id: item1

    Pane {
        id: pane1
        anchors.fill: parent

        GroupBox {
            id: groupBox2
            anchors.top: globalTXIndicator.bottom
            anchors.topMargin: 0
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            title: qsTr("Devices")

            ColumnLayout {
                id: columnLayout1
                anchors.fill: parent
            }
        }

        StatusIndicator {
            id: globalTXIndicator
            active: false
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.top: parent.top
            anchors.topMargin: 0
        }

        Text {
            id: text1
            x: 97
            y: 7
            width: 116
            height: 14
            text: qsTr("Text")
            font.pixelSize: 12
        }
    }
}
