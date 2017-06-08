import QtQuick 2.4
import Qt.labs.settings 1.0
import QtQuick.Controls 2.0
import QtQuick.Extras 1.4
import QtQuick.Layouts 1.0

Item {

    Pane {
        id: pane1
        anchors.fill: parent

        GridLayout {
            anchors.fill: parent
            rows: 3
            columns: 2

            Text {
                id: text1
                text: qsTr("IP/HOST:")
                font.pointSize: 20
            }

            TextInput {
                id: hostInput
                text: qsTr("192.168.4.1")
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                font.pointSize: 25
            }

            Text {
                id: text2
                text: qsTr("PORT:")
                font.pointSize: 20
            }

            TextInput {
                id: portInput
                text: qsTr("31415")
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                font.pointSize: 25
            }

            BusyIndicator {
                id: busyIndicator1
                spacing: 0
                enabled: true
                running: false
            }

            Button {
                id: connectBtn
                text: qsTr("Connect!")
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            }
        }
    }
}
