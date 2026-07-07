#include <QtQuick>
#include "audiorecorder.h"
#include "notesmodel.h"
#include "transcriptioncontroller.h"
#include <auroraapp.h>

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> application(Aurora::Application::application(argc, argv));
    application->setOrganizationName(QStringLiteral("ru.repeater"));
    application->setApplicationName(QStringLiteral("repeater"));

    // СОЗДАЕМ В КУЧЕ! И передаем приложение как parent, чтобы Qt сам удалил их при выходе
    AudioRecorder *audioRecorder = new AudioRecorder(application.data());
    NotesModel *notesModel = new NotesModel(application.data());
    TranscriptionController *sttController = new TranscriptionController(application.data());

    // Подключение сигналов
    QObject::connect(sttController,
                     &TranscriptionController::transcriptionFinished,
                     notesModel,
                     &NotesModel::addNoteFromTranscription);

    QScopedPointer<QQuickView> view(Aurora::Application::createView());
    view->rootContext()->setContextProperty(QStringLiteral("audioRecorder"), audioRecorder);
    view->rootContext()->setContextProperty(QStringLiteral("notesModel"), notesModel);
    view->rootContext()->setContextProperty(QStringLiteral("sttController"), sttController);

    view->setSource(Aurora::Application::pathTo(QStringLiteral("qml/repeater.qml")));
    view->show();

    return application->exec();
}
