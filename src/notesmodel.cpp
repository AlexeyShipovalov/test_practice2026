#include "notesmodel.h"
#include <QStandardPaths>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

NotesModel::NotesModel(QObject *parent) : QAbstractListModel(parent)
{
    m_dataFilePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/notes.json";
    loadNotes();
}

int NotesModel::rowCount(const QModelIndex &parent) const { return m_notes.size(); }

QVariant NotesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return {};
    const Note &n = m_notes[index.row()];
    switch (role) {
    case TitleRole: return n.title;
    case DateRole: return n.date;
    case AudioPathRole: return n.audioPath;
    case TextRole: return n.text;
    }
    return {};
}

QHash<int, QByteArray> NotesModel::roleNames() const
{
    return { {TitleRole, "title"}, {DateRole, "date"}, {AudioPathRole, "audioPath"}, {TextRole, "text"} };
}

void NotesModel::addNoteFromTranscription(const QString &title, const QString &text, const QString &audioPath)
{
    beginInsertRows(QModelIndex(), 0, 0);
    Note n;
    n.title = title.isEmpty() ? "Запись " + QDateTime::currentDateTime().toString("dd.MM.yy hh:mm") : title;
    n.date = QDateTime::currentDateTime().toString(Qt::ISODate);
    n.audioPath = audioPath;
    n.text = text;
    m_notes.prepend(n);
    endInsertRows();
    emit countChanged();
    saveNotes();
}

void NotesModel::removeNote(int row)
{
    if (row < 0 || row >= m_notes.size()) return;
    beginRemoveRows(QModelIndex(), row, row);
    m_notes.removeAt(row);
    endRemoveRows();
    emit countChanged();
    saveNotes();
}

void NotesModel::saveNotes()
{
    QJsonArray arr;
    for (const auto &n : m_notes) {
        QJsonObject obj;
        obj["title"] = n.title;
        obj["date"] = n.date;
        obj["audio"] = n.audioPath;
        obj["text"] = n.text;
        arr.append(obj);
    }
    QFile f(m_dataFilePath);
    if (f.open(QIODevice::WriteOnly)) f.write(QJsonDocument(arr).toJson());
}

void NotesModel::loadNotes()
{
    QFile f(m_dataFilePath);
    if (!f.open(QIODevice::ReadOnly)) return;
    QJsonArray arr = QJsonDocument::fromJson(f.readAll()).array();
    for (const auto &val : arr) {
        QJsonObject obj = val.toObject();
        Note n;
        n.title = obj["title"].toString();
        n.date = obj["date"].toString();
        n.audioPath = obj["audio"].toString();
        n.text = obj["text"].toString();
        m_notes.append(n);
    }
}
