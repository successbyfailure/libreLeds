import QtQuick 2.4
import QtQuick.Controls.Universal 2.0
import QtQuick.Extras 1.4
import Qt.labs.settings 1.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Item {
    height: 100

    GroupBox {
        id: ledGadgetGroupBox
        anchors.fill: parent
        title: qsTr("ledGadget")

        RowLayout {
            anchors.fill: parent

            StatusIndicator {
                id: statusIndicator1
            }

            Picture {
                id: picture1
                Layout.preferredHeight: 41
                Layout.preferredWidth: 42
                source: "images/unknown.png"
            }

            Button {
                id: flashBtn
                text: qsTr("Flash")
                Layout.preferredHeight: 24
                Layout.preferredWidth: 56
            }

            Button {
                id: fadeBtn
                text: qsTr("Fade")
                Layout.preferredHeight: 24
                Layout.preferredWidth: 56
            }

            Button {
                id: effectsBtn
                text: qsTr("Effects")
                Layout.preferredHeight: 24
                Layout.preferredWidth: 62
            }

            Button {
                id: configBtn
                text: qsTr("Config")
                Layout.preferredHeight: 24
                Layout.preferredWidth: 56
            }

            DelayButton {
                id: removeBtn
                text: qsTr("Remove")
                Layout.preferredHeight: 56
                Layout.preferredWidth: 48
                delay: 2000
            }
        }
    }
}
