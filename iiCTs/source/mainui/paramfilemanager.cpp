#include "paramfilemanager.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>

ParamFileManager::ParamFileManager(
        QComboBox                       *fileCombo,
        const QVector<ValueParam>       &valueParams,
        const QVector<FileParam>        &fileParams,
        const QMap<QString, QLineEdit*> &valueEdits,
        const QMap<QString, QLineEdit*> &fileEdits,
        QObject                         *parent)
    : QObject(parent)
    , m_fileCombo(fileCombo)
    , m_valueEdits(valueEdits)
    , m_fileEdits(fileEdits)
    , m_initialValueParams(valueParams)
    , m_initialFileParams(fileParams)
    , m_currentValueParams(valueParams)
    , m_currentFileParams(fileParams)
{
    connect(m_fileCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &ParamFileManager::loadFileAtIndex);

    for (auto it = m_valueEdits.constBegin(); it != m_valueEdits.constEnd(); ++it) {
        QString key   = it.key();
        QLineEdit *e  = it.value();
        connect(e, &QLineEdit::editingFinished, this,
                [this, key, e](){
            applyAllUpdates(key, e->text());
        });
    }

    for (auto it = m_fileEdits.constBegin(); it != m_fileEdits.constEnd(); ++it) {
        QString key   = it.key();
        QLineEdit *e  = it.value();
        connect(e, &QLineEdit::textChanged, this,
                [this, key](const QString &txt){
            applyAllUpdates(key, txt);
        });
    }
}


void ParamFileManager::loadFileAtIndex(int idx)
{
    if (idx < 0) return;

    m_pendingUpdates.clear();

    m_currentValueParams = m_initialValueParams;
    m_currentFileParams  = m_initialFileParams;

    for (auto &vp : m_currentValueParams)
        if (m_valueEdits.contains(vp.name))
            m_valueEdits[vp.name]->setText(QString::number(vp.defaultValue));

    for (auto &fp : m_currentFileParams)
        if (m_fileEdits.contains(fp.name))
            m_fileEdits[fp.name]->setText(fp.filePath);

    m_currentFilePath = m_fileCombo->itemData(idx, Qt::UserRole).toString();
    QFile file(m_currentFilePath);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) return;
    QTextStream in(&file);
    in.setAutoDetectUnicode(true);
    m_fileContentList.clear();
    while (!in.atEnd())
        m_fileContentList << in.readLine();
    file.close();

    m_nameMap.clear();
    QRegularExpression reMap(R"(^\s*([^#\s]+))");
    for (auto &ln : m_fileContentList) {
        auto m = reMap.match(ln);
        if (!m.hasMatch()) continue;
        QString raw      = m.captured(1);
        QString friendly = raw.replace('-', ' ');
        m_nameMap[friendly] = raw;
    }

    parseParamsFromLines();
    refreshUIFromParams();
}

void ParamFileManager::reloadCurrentFile()
{
    loadFileAtIndex(m_fileCombo->currentIndex());
}

void ParamFileManager::parseParamsFromLines()
{
    QRegularExpression re(R"(^\s*([^#\s]+)\s+([^#\s]+))");
    for (const auto &ln : m_fileContentList) {
        QString t = ln.trimmed();
        if (t.isEmpty() || t.startsWith('#')) continue;
        auto m = re.match(ln);
        if (!m.hasMatch()) continue;

        QString rawName  = m.captured(1);
        QString rawValue = m.captured(2);
        QString name     = rawName.replace('-', ' ');

        bool ok = false;
        double v = rawValue.toDouble(&ok);
        if (ok) {
            for (auto &vp : m_currentValueParams)
                if (vp.name == name) vp.defaultValue = v;
        }
        for (auto &fp : m_currentFileParams)
            if (fp.name == name) fp.filePath = rawValue;
    }
}

void ParamFileManager::refreshUIFromParams()
{
    for (const auto &vp : m_currentValueParams) {
        if (m_valueEdits.contains(vp.name))
            m_valueEdits[vp.name]->setText(QString::number(vp.defaultValue));
    }
    for (const auto &fp : m_currentFileParams) {
        if (m_fileEdits.contains(fp.name))
            m_fileEdits[fp.name]->setText(fp.filePath);
    }
}


void ParamFileManager::replaceOneParam(QStringList &lines,
                                       const QString &paramName,
                                       const QString &newValue)
{
    QString rawName = m_nameMap.value(paramName, paramName);

    const QString pat = QStringLiteral(
                R"(^(\s*)%1(\s+)[^\s#]+(.*)$)"
                ).arg(QRegularExpression::escape(rawName));
    QRegularExpression re(pat);

    for (int i = 0; i < lines.size(); ++i) {
        auto m = re.match(lines[i]);
        if (!m.hasMatch()) continue;
        QString lead  = m.captured(1),
                sep   = m.captured(2),
                trail = m.captured(3);
        lines[i] = lead + rawName + sep + newValue + trail;
    }
}

void ParamFileManager::applyAllUpdates(const QString &key, const QString &newValue)
{
    m_pendingUpdates[key] = newValue;
    replaceOneParam(m_fileContentList, key, newValue);
    emit fileContentListChanged(m_fileContentList);
}


