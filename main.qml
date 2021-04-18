import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Key Wallet")
    color: "black"
    Component {
        id: vwAddUser
        AddUser {
            onRegisterAttempt: {
                var userAdded = kw_session.add_user(user, pass)
                console.log(`userAdded: ${userAdded}`)
                if (userAdded) {
                    stackView.pop()
                }
            }
        }
    }
    Component {
        id: vwLogin
        Login {
            onLoginAttempt: {
                var loginSuccess = kw_session.authenticate(user, pass)
                console.log(`loginSuccess: ${loginSuccess}`)
                if (loginSuccess) {
                    stackView.push(vwUserPortal)
                }
            }
        }
    }
    Component {
        id: vwUserPortal
        UserPortal {

        }
    }

    //    function pushBackViewByName(name) {
    //        var component = Qt.createComponent(name);
    //        if (component.status == Component.Ready) {
    //            var view = component.createObject(stackView);
    //            stackView.push(view);
    //        }
    //    }
    Component.onCompleted: {
        stackView.push(vwLogin)
        if (kw_session.is_first_session()) {
            stackView.push(vwAddUser)
        }
//        stackView.push(vwUserPortal)
    }

    StackView {
        id: stackView
        anchors.fill: parent
    }
}
