#pragma once
#include <QAbstractListModel>
#include <QDateTime>

struct Note
{
    QString title;
    QString date;
    QString audioPath;
    QString text;
};

class NotesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
public:
    enum Roles { TitleRole = Qt::UserRole + 1, DateRole, AudioPathRole, TextRole };

    explicit NotesModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void addNoteFromTranscription(const QString &title,
                                              const QString &text,
                                              const QString &audioPath);
    Q_INVOKABLE void removeNote(int row);
    Q_INVOKABLE void saveNotes();
    Q_INVOKABLE void loadNotes();
    Q_INVOKABLE void exportNoteToFile(int row);
signals:
    void countChanged();

private:
    QList<Note> m_notes;
    QString m_dataFilePath;
};
