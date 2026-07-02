TARGET = ru.repeater.repeater


CONFIG += auroraapp auroraapp_i18n
CONFIG += link_pkgconfig

QMAKE_RPATHDIR += /usr/share/$$TARGET/lib


PKGCONFIG += auroraapp
PKGCONFIG += onnxruntime  Qt5Multimedia

SOURCES += src/main.cpp \
    src/audiorecorder.cpp \
    src/notesmodel.cpp \
    src/sttworker.cpp \
    src/transcriptioncontroller.cpp \

HEADERS += src/audiorecorder.h \
    src/notesmodel.h \
    src/sttworker.h \
    src/transcriptioncontroller.h \

DISTFILES += rpm/ru.repeater.repeater.spec \
    qml/repeater.qml \
    qml/pages/MainPage.qml \
    qml/pages/RecordPage.qml \
    qml/pages/NoteViewPage.qml \
    qml/cover/DefaultCoverPage.qml

AURORAAPP_ICONS = 86x86 108x108 128x128 172x172
