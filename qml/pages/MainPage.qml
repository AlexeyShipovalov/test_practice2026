import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: mainPage
    allowedOrientations: Orientation.All

    SilicaListView {
        id: listView
        anchors.fill: parent
        model: notesModel

        PullDownMenu {
            MenuItem {
                text: qsTr("Новая запись")
                onClicked: pageStack.push(Qt.resolvedUrl("RecordPage.qml"))
            }
        }

        header: Column {
            width: parent.width
            spacing: Theme.paddingSmall

            PageHeader { title: qsTr("Мои заметки") }

            Label {
                id: modelStatusLabel
                x: Theme.horizontalPageMargin
                width: parent.width - 2*Theme.horizontalPageMargin
                font.pixelSize: Theme.fontSizeExtraSmall
                color: sttController.isModelReady ? Theme.secondaryColor : Theme.highlightColor
                text: qsTr("Загрузка модели...")
                wrapMode: Text.WordWrap
            }

            Connections {
                target: sttController
                onModelReadyChanged: {
                    if (sttController.isModelReady) {
                        modelStatusLabel.text = qsTr("Модель речи готова")
                        modelStatusLabel.color = Theme.secondaryColor
                    }
                }
                onErrorOccurred: {
                    modelStatusLabel.text = qsTr("Ошибка: ") + err
                    modelStatusLabel.color = "red"
                }
            }

            SearchField {
                id: searchField
                width: parent.width
                placeholderText: qsTr("Поиск по тексту...")
            }
        }

        ViewPlaceholder {
            enabled: listView.count === 0
            text: qsTr("Заметок нет")
            hintText: qsTr("Создайте новую запись")
        }

        delegate: ListItem {
            id: delegate
            width: parent.width
            contentHeight: visible ? Theme.itemSizeMedium : 0

            // Правильная фильтрация поиска (без пустых строк)
            visible: searchField.text.length === 0 ||
                     model.text.toLowerCase().includes(searchField.text.toLowerCase())

            menu: ContextMenu {
                MenuItem {
                    text: qsTr("Удалить")
                    onClicked: delegate.remorseDelete(function() { notesModel.removeNote(index) })
                }
            }

            onClicked: pageStack.push(Qt.resolvedUrl("NoteViewPage.qml"), {
                noteTitle: model.title,
                noteText: model.text,
                audioPath: model.audioPath
            })

            Column {
                anchors.left: parent.left; anchors.right: parent.right
                anchors.leftMargin: Theme.horizontalPageMargin
                anchors.rightMargin: Theme.horizontalPageMargin
                anchors.verticalCenter: parent.verticalCenter

                Label {
                    width: parent.width
                    text: model.title
                    color: delegate.highlighted ? Theme.highlightColor : Theme.primaryColor
                    truncationMode: TruncationMode.Fade
                }
                Label {
                    width: parent.width
                    // Исправлено: Qt.parseDateTime нет, используем new Date
                    text: Qt.formatDateTime(new Date(model.date), "dd MMM yyyy, hh:mm")
                    font.pixelSize: Theme.fontSizeSmall
                    color: Theme.secondaryColor
                }
            }
        }
    }

    // Панель фоновой расшифровки внизу экрана
    Item {
        id: transcribingPanel
        anchors.bottom: parent.bottom
        width: parent.width
        height: sttController.isTranscribing ? column.height + Theme.paddingLarge : 0
        visible: sttController.isTranscribing
        clip: true

        BackgroundItem {
            anchors.fill: parent
            onClicked: {
                // Отмена по тапу на панель (опционально)
                // sttController.cancelTranscription()
            }
        }

        Column {
            id: column
            width: parent.width - 2 * Theme.horizontalPageMargin
            x: Theme.horizontalPageMargin
            spacing: Theme.paddingSmall

            Label {
                width: parent.width
                text: qsTr("Расшифровка записи...")
                color: Theme.highlightColor
            }

            ProgressBar {
                width: parent.width
                minimumValue: 0
                maximumValue: 100
                value: sttController.progress
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Отменить")
                onClicked: sttController.cancelTranscription()
                color: "red"
            }
        }
    }
}
