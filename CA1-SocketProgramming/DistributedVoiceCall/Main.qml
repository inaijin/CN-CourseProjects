import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: window
    visible: true
    width: 280
    height: 520
    title: qsTr("WebRTC Voice Call")

    // Property to track call status
    property bool isCalling: false

    // Layout
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 10

        // Role selection
        RowLayout {
            spacing: 10
            Label {
                text: "Role:"
                Layout.alignment: Qt.AlignVCenter
            }
            ComboBox {
                id: roleComboBox
                Layout.fillWidth: true
                model: ["caller", "callee"]
                currentIndex: 0
                onCurrentTextChanged: {
                    callManager.role = currentText
                    hostRow.visible = (currentText === "caller") // Ensuring visibility only when caller is selected
                }
            }
        }

        // Host input (visible only if role is caller)
        RowLayout {
            id: hostRow
            visible: true // Default to visible for caller
            spacing: 10
            Label {
                text: "Host:"
                Layout.alignment: Qt.AlignVCenter
            }
            TextField {
                id: hostField
                Layout.fillWidth: true
                placeholderText: "127.0.0.1"
                text: "127.0.0.1"
                onTextChanged: {
                    callManager.host = text
                }
            }
        }

        // Port input
        RowLayout {
            spacing: 10
            Label {
                text: "Port:"
                Layout.alignment: Qt.AlignVCenter
            }
            TextField {
                id: portField
                Layout.fillWidth: true
                placeholderText: "12345"
                text: "12345"
                inputMethodHints: Qt.ImhDigitsOnly
                onTextChanged: {
                    var port = parseInt(text)
                    if (!isNaN(port)) {
                        callManager.port = port
                    }
                }
            }
        }

        // Status display
        Label {
            id: statusLabel
            text: "Status: " + callManager.status
            Layout.fillWidth: true
            wrapMode: Text.Wrap
            color: "blue"
        }

        // Call button
        Button {
            id: callBtn
            height: 47
            text: isCalling ? "End Call" : "Call"
            background: Rectangle {
                color: isCalling ? "red" : "green"
                radius: 5
            }
            contentItem: Text {
                text: callBtn.text
                color: "white"
                font.bold: true
                anchors.centerIn: parent
            }
            Layout.fillWidth: true

            onClicked: {
                if (isCalling) {
                    callManager.endCall()
                    isCalling = false
                    callBtn.text = "Call"
                    callBtn.background.color = "green"
                } else {
                    if (roleComboBox.currentText === "caller" && hostField.text === "") {
                        statusLabel.text = "Please enter a host address."
                        return
                    }
                    callManager.startCall()
                    isCalling = true
                    callBtn.text = "End Call"
                    callBtn.background.color = "red"
                }
            }
        }
    }
}
