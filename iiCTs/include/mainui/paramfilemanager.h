#ifndef PARAMFILEMANAGER_H
#define PARAMFILEMANAGER_H

#include <QObject>
#include <QStringList>
#include <QVector>
#include <QMap>
#include <QLineEdit>
#include <QComboBox>

struct ValueParam { QString name; double defaultValue; QString unit; };
struct FileParam  { QString name; QString filePath; };

class ParamFileManager : public QObject {
    Q_OBJECT

public:
    ParamFileManager(QComboBox                       *fileCombo,
                     const QVector<ValueParam>       &valueParams,
                     const QVector<FileParam>        &fileParams,
                     const QMap<QString, QLineEdit*> &valueEdits,
                     const QMap<QString, QLineEdit*> &fileEdits,
                     QObject                         *parent = nullptr);

signals:
    void fileContentListChanged(const QStringList &newContent);

public slots:
    void reloadCurrentFile();

private:
    void loadFileAtIndex(int idx);

    QComboBox                        *m_fileCombo;
    QMap<QString, QLineEdit*>        m_valueEdits;
    QMap<QString, QLineEdit*>        m_fileEdits;

    QVector<ValueParam>              m_initialValueParams;
    QVector<FileParam>               m_initialFileParams;

    QVector<ValueParam>              m_currentValueParams;
    QVector<FileParam>               m_currentFileParams;

    QStringList                      m_fileContentList;
    QMap<QString, QString>           m_pendingUpdates;
    QMap<QString, QString>           m_nameMap;
    QString                          m_currentFilePath;

    void parseParamsFromLines();
    void refreshUIFromParams();
    void replaceOneParam(QStringList &lines,
                         const QString &paramName,
                         const QString &newValue);
    void applyAllUpdates(const QString &key, const QString &newValue);
};

#endif // PARAMFILEMANAGER_H
