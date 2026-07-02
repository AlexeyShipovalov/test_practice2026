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
            contentHeight: Theme.itemSizeMedium

            // Фильтрация поиска
            opacity: model.text.toLowerCase().includes(searchField.text.toLowerCase()) ? 1.0 : 0.0
            visible: opacity > 0.0

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
                    text: Qt.formatDateTime(Qt.parseDateTime(model.date), "dd MMM yyyy, hh:mm")
                    font.pixelSize: Theme.fontSizeSmall
                    color: Theme.secondaryColor
                }
            }
        }
    }
}
