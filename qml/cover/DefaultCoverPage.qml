import QtQuick 2.0
import Sailfish.Silica 1.0

CoverBackground {
    Column {
        anchors.centerIn: parent
        width: parent.width
        spacing: Theme.paddingMedium

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Диктофон")
            color: Theme.highlightColor
        }
        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("%1 заметок").arg(notesModel.count)
            color: Theme.secondaryColor
        }
    }
}
