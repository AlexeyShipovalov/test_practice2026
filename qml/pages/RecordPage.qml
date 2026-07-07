import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: recordPage
    allowedOrientations: Orientation.All

    Column {
        id: column
        width: parent.width
        anchors.centerIn: parent
        spacing: Theme.paddingLarge

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: audioRecorder.duration
            font.pixelSize: Theme.fontSizeExtraLarge
            color: Theme.highlightColor
        }

        ProgressBar {
            width: parent.width - 2*Theme.horizontalPageMargin
            minimumValue: 0
            maximumValue: 100
            value: audioRecorder.level
            label: qsTr("Уровень сигнала")
        }

        Button {
            id: recButton
            anchors.horizontalCenter: parent.horizontalCenter
            text: audioRecorder.recording ? qsTr("Стоп") : qsTr("Старт")
            preferredWidth: Theme.buttonWidthLarge
            color: audioRecorder.recording ? "red" : Theme.primaryColor

            onClicked: {
                if (audioRecorder.recording) {
                    var path = audioRecorder.stopRecording()
                    sttController.startTranscription(path, "Новая лекция")
                    // Возвращаемся на главную, где увидим прогресс расшифровки
                    pageStack.pop()
                } else {
                    audioRecorder.startRecording()
                }
            }
        }
    }
}
