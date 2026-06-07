#include "progressmonitor.h"
#include <QPlainTextEdit>
#include <QTextDocument>
#include <QRegularExpression>
#include <QDebug>

ProgressMonitor::ProgressMonitor(QPlainTextEdit* editor, QObject* parent)
    : QObject(parent),
      m_editor(editor),
      m_lastPos(0)
{
    connect(m_editor->document(),
            &QTextDocument::contentsChanged,
            this,
            &ProgressMonitor::onContentsChanged);
}

ProgressMonitor::~ProgressMonitor() = default;

void ProgressMonitor::onContentsChanged()
{
    const QString all = m_editor->toPlainText();
    const int newLength = all.length();
    if (newLength <= m_lastPos) return;

    const QString added = all.mid(m_lastPos);
    m_lastPos = newLength;

    const QStringList lines = added.split('\n', QString::SkipEmptyParts);
    for (const QString& rawLine : lines) {
        QString line = rawLine.trimmed();
        if (line.contains("[ProgressSTART]")) {
            QString text = line.trimmed();
                if (text.startsWith(">>> ")) {
                    text = text.mid(4).trimmed();
                }
                static const QRegularExpression re(R"(\[\s*Name:\s*([^\]\s]+)\s*\]\s*\[\s*MAX:\s*(\d+)\s*\])");
            QRegularExpressionMatch match = re.match(text);
            if (match.hasMatch()) {
                QString name = match.captured(1);
                int max = match.captured(2).toInt();
                emit progressTaskStarted(name, max);
            }
        }
        else if (line.contains("iiCTs:")) {
            QRegularExpression re(R"(iiCTs:(\d+))");
            QRegularExpressionMatch match = re.match(line);
            if (match.hasMatch()) {
                int val = match.captured(1).toInt();
                emit progressUpdated(val);
            }
        }
    }
}
