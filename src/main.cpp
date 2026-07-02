#include <auroraapp.h>
#include <QtQuick>
#include "audiorecorder.h"
#include "transcriptioncontroller.h"
#include "notesmodel.h"

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> application(Aurora::Application::application(argc, argv));
    application->setOrganizationName(QStringLiteral("ru.repeater"));
    application->setApplicationName(QStringLiteral("repeater"));

    // Регистрация C++ объектов в QML как контекстные свойства
    AudioRecorder audioRecorder;
    NotesModel notesModel;
    TranscriptionController sttController;

    // Подключение сигналов: STT расшифровал текст -> Модель добавляет заметку
    QObject::connect(&sttController, &TranscriptionController::transcriptionFinished,
                     &notesModel, &NotesModel::addNoteFromTranscription);

    QScopedPointer<QQuickView> view(Aurora::Application::createView());
    view->rootContext()->setContextProperty(QStringLiteral("audioRecorder"), &audioRecorder);
    view->rootContext()->setContextProperty(QStringLiteral("notesModel"), &notesModel);
    view->rootContext()->setContextProperty(QStringLiteral("sttController"), &sttController);

    view->setSource(Aurora::Application::pathTo(QStringLiteral("qml/repeater.qml")));
    view->show();

    return application->exec();
}
