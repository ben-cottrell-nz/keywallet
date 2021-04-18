import QtQuick 2.12
import QtQuick.Controls 2.15
import Qt.labs.qmlmodels 1.0

Item {

    ScrollView {
        id: scrollView
        anchors.fill: parent
        clip: true
        TableView {
            anchors.fill: parent
            columnSpacing: 1
            rowSpacing: 1
            clip: true
            model: kw_session.credential_model()
        }
    }
}
