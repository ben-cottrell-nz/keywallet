import QtQuick 2.0
import QtQuick.Controls 2.12

Item {
    signal registerAttempt(string user, string pass)
    Image {
        id: image
        anchors.fill: parent
        source: "wallet-man-backpocket.jpeg"
        fillMode: Image.PreserveAspectCrop

        Rectangle {
            id: rectangle
            x: 334
            y: 55
            width: 262
            height: 396
            color: "#c5000000"
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter

            Label {
                id: label
                x: 53
                y: 43
                color: "#ffffff"
                text: qsTr("Add User")
                font.pointSize: 25
            }

            Label {
                id: lblUsername
                x: 19
                y: 109
                color: "#ffffff"
                text: qsTr("Username")
                font.pointSize: 14
            }

            TextField {
                id: tfUser
                x: 21
                y: 141
                placeholderText: qsTr("Username")
            }

            Label {
                id: lblPassword
                x: 23
                y: 210
                color: "#ffffff"
                text: qsTr("Password")
                font.pointSize: 14
            }

            TextField {
                id: tfPassword
                x: 25
                y: 242
                placeholderText: qsTr("Password")
                passwordCharacter: '\u2022'
                echoMode: TextInput.Password
            }

            Button {
                id: btnRegister
                x: 116
                y: 320
                text: qsTr("Register")
                font.pointSize: 14
                onClicked: registerAttempt(tfUser.text, tfPassword.text)
            }
        }
    }
}
