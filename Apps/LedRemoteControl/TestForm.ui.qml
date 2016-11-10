import QtQuick 2.4

Item {
    width: 400
    height: 400

    TextInput {
        id: textInput1
        x: 68
        y: 57
        width: 80
        height: 20
        text: qsTr("Text Input")
        font.pixelSize: 12
    }

    Text {
        id: text1
        x: 95
        y: 118
        text: qsTr("Text")
        font.pixelSize: 12
    }
    states: [
        State {
            name: "State1"
        },
        State {
            name: "State2"
        }
    ]
}
