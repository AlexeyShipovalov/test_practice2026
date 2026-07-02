import QtQuick 2.0
import Sailfish.Silica 1.0
import QtMultimedia 5.0

Page {
    id: noteViewPage
    property string noteTitle
    property string noteText
    property string audioPath

    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: content.height

        PullDownMenu {
            MenuItem {
                text: qsTr("Копировать текст")
                onClicked: Clipboard.text = noteText
            }
            MenuItem {
                text: qsTr("Экспорт в файл")
                onClicked: {
                    // Сохранение в Загрузки (требует Permissions=Downloads)
                    var path = StandardPaths.download + "/export_" + Date.now() + ".txt"
                    // Для реального сохранения потребуется C++ метод или FileIO
                    console.log("Exporting to: " + path)
                }
            }
        }

        Column {
            id: content
            width: parent.width
            spacing: Theme.paddingMedium

            PageHeader { title: noteTitle }

            Audio {
                id: audioPlayer
                source: audioPath
            }

            Row {
                spacing: Theme.paddingMedium
                anchors.horizontalCenter: parent.horizontalCenter
                Button {
                    text: audioPlayer.playbackState == Audio.PlayingState ? qsTr("Пауза") : qsTr("Слушать")
                    onClicked: {
                        if (audioPlayer.playbackState == Audio.PlayingState) audioPlayer.pause()
                        else audioPlayer.play()
                    }
                }
            }

            Slider {
                width: parent.width
                minimumValue: 0
                maximumValue: audioPlayer.duration
                value: audioPlayer.position
                onValueChanged: if (pressed) audioPlayer.seek(value)
                label: "Воспроизведение"
            }

            Text {
                width: parent.width - 2*Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                wrapMode: Text.WordWrap
                color: Theme.primaryColor
                font.pixelSize: Theme.fontSizeSmall
                text: noteText
            }
        }
    }
}
